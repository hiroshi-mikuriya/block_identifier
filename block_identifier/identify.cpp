#include "identify.h"
#include "define.h"

namespace {
	/*!
	上端、下端
	*/
	struct TopBottom
	{
		int top;
		int bottom;
	};

	/*!
	輪郭2値画像の上端、下端を返す
	ブロックの上ボッチをなるべく消す
	@param[in] bin 輪郭2値画像
	@return 上端、下端
	*/
	TopBottom getTopBottom(cv::Mat const & bin)
	{
		cv::Mat m;
		cv::reduce(bin, m, 1, CV_REDUCE_AVG);
		m = m.t();
		TopBottom dst;
		const int th = 40; // TODO: ちゃんと計算しろ
		for (dst.top = 0; m.data[dst.top] < th; dst.top++);
		for (dst.bottom = m.cols - 1; m.data[dst.bottom] < th; dst.bottom--);
		return dst;
	}

	/*!
	矩形を縮尺率を変換する。中心は保つ。
	@param[in] rc 矩形
	@param[in] r 縮尺率
	@return 変換された矩形
	*/
	cv::Rect operator*(cv::Rect const & rc, double r)
	{
		return cv::Rect(
			static_cast<int>(rc.x + rc.width * (1 - r) / 2),
			static_cast<int>(rc.y + rc.height * (1 - r) / 2),
			std::max(1, static_cast<int>(rc.width * r)),
			std::max(1, static_cast<int>(rc.height * r))
			);
	}

	/*!
	画像からレゴブロックを認識するクラス
	*/
	class IdentifyBlock
	{
		cv::Mat const image_;
		std::vector<Color> const colors_;

		/*!
		ブロック情報の画像を表示する
		@param[in] blockInfo
		*/
		void showBlockInfo(std::vector<BlockInfo> const & blockInfo)
		{
			cv::Mat canvas = cv::Mat::zeros(image_.rows, image_.cols * 2, CV_8UC3);
			image_.copyTo(canvas(cv::Rect(0, 0, image_.cols, image_.rows)));
			for (auto info : blockInfo){
				cv::rectangle(canvas, info.rc, cv::Scalar(0, 255, 0), 1);
				cv::rectangle(canvas, info.color_area, cv::Scalar(255, 0, 255), 1);
				auto v = info.color.bgr;
				std::stringstream ss;
				ss << info.type << ":" << info.color.name << " ";
				ss.width(2);
				ss.fill('0');
				ss << std::hex << (int)info.ave[2] << (int)info.ave[1] << (int)info.ave[0];
				cv::putText(canvas, ss.str(), cv::Point(image_.cols * 1.1, info.rc.y + info.rc.height * 0.7), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(v[0], v[1], v[2]));
			}
			cv::imshow("blocks", canvas);
		}
		/*!
		このプログラムが認識する色の中で最も近い色を返す
		@param[in] bgr BGR値
		@return 最も近い色
		*/
		Color getColor(cv::Vec3b bgr)
		{
			double len2 = 100000;
			Color dst;
			auto calcLen2 = [](cv::Vec3b v0, cv::Vec3b v1){
				double d = 0;
				for (int i = 0; i < 3; ++i){
					d += (v0[i] - v1[i]) * (v0[i] - v1[i]);
				}
				return d;
			};
			for (auto c : colors_){
				auto len = calcLen2(bgr, c.bgr);
				if (len < len2){
					len2 = len;
					dst = c;
				}
			}
			return dst;
		}
		/*!
		カメラの画像からブロックの輪郭を抽出する
		@param[in] m カメラ画像(RGB)
		@return ブロックの輪郭
		*/
		std::vector<cv::Point> getBlockContour()
		{
			cv::Mat hls;
			cv::cvtColor(image_, hls, CV_BGR2HLS);
			std::vector<cv::Mat> v;
			cv::split(hls, v);
			auto l = v[1];
			auto s = v[2];
			double slratio = 0.5;
			auto mixed = slratio * l + (1 - slratio) * s;
			cv::Mat block;
			cv::threshold(mixed, block, 80, 255, cv::THRESH_BINARY);
			typedef std::vector<cv::Point> contour_t;
			std::vector<contour_t> contours;
			cv::findContours(block, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			// 一番面積の広い領域がブロックと判断する。ただし画像サイズ並みの面積だった場合は除外
			contour_t cach;
			double maxArea = -1;
			for (auto points : contours){
				double area = cv::contourArea(points);
				if (maxArea < area && area < image_.size().area() * 0.9){
					maxArea = area;
					cach = points;
				}
			}
			return cach;
		}
		/*!
		画像内の指定した矩形の全ブロック情報を取得する
		@param[in] points ブロックの矩形
		@return ブロック情報
		*/
		std::vector<BlockInfo> getBlockInfo(std::vector<cv::Point> const & points)
		{
			auto const bin = [this, &points](){
				cv::Mat bin = cv::Mat::zeros(image_.size(), CV_8UC1);
				std::vector<std::vector<cv::Point>> contours = { points };
				cv::drawContours(bin, contours, 0, 255, CV_FILLED);
				return bin;
			}();
			auto getAveBgr = [](cv::Mat const & src){
				cv::Mat tmp;
				cv::reduce(src, tmp, 1, CV_REDUCE_AVG);
				cv::reduce(tmp, tmp, 0, CV_REDUCE_AVG);
				return tmp.at<cv::Vec3b>(0);
			};
			auto tb = getTopBottom(bin);
			std::vector<BlockInfo> dst;
			if (tb.bottom <= tb.top){
				return dst;
			}
			int blockCount = (tb.bottom - tb.top + BLOCK_SIZE / 2) / BLOCK_SIZE;
			for (int i = 0; i < blockCount; ++i){
				int y = (tb.top * (blockCount - i) + tb.bottom * i) / blockCount;
				cv::Mat ary;
				cv::reduce(bin(cv::Rect(0, y, image_.cols, BLOCK_SIZE)), ary, 0, CV_REDUCE_AVG);
				int left = 0;
				const int th = 220;
				for (; ary.data[left] < th && left < ary.cols; ++left);
				int right = bin.cols - 1;
				for (; ary.data[right] < th && 0 <= right; --right);
				if (right <= left) continue; // 計算できなかったので仕方ないからあきらめる
				BlockInfo info;
				info.rc = cv::Rect(left, y, right - left, BLOCK_SIZE);
				info.color_area = info.rc * 0.2;
				info.ave = getAveBgr(image_(info.color_area));
				info.color = getColor(info.ave);
				info.type = (right - left + BLOCK_SIZE_WIDTH / 2) / BLOCK_SIZE_WIDTH;
				dst.push_back(info);
			}
			return dst;
		}
	public:
		IdentifyBlock(
			cv::Mat const & image,
			std::vector<Color> const & colors,
			std::vector<BlockInfo> & blockInfo)
			: image_(image)
			, colors_(colors)
		{
			assert(3 == m.channels());
			auto const contour = getBlockContour();
			blockInfo = getBlockInfo(contour);
			showBlockInfo(blockInfo);
		}
	};
}

void identifyBlock(
	cv::Mat const & image,
	std::vector<Color> const & colors,
	std::vector<BlockInfo> & blockInfo)
{
	IdentifyBlock(image, colors, blockInfo);
}