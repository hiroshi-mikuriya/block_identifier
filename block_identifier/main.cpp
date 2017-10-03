#include <iostream>
#include <opencv2/opencv.hpp>
#include <cassert>

/// カメラ横サイズ
const int CAMERA_WIDTH = 1280;
/// カメラ縦サイズ
const int CAMERA_HEIGHT = 720;
/// 画像縮尺率
const double IMAGE_RATIO = 0.3;
/// IMAGE_RATIOをかけたあとのブロックサイズ（高さ）
const int BLOCK_SIZE = static_cast<int>(102 * IMAGE_RATIO);

struct Color
{
    std::string name;
    cv::Vec3b ycc;
};

Color const colors[] = {
    { "red", {56, 184, 104} },
    { "green", {110, 102, 106} },
    { "white", {182, 130, 121} },
    { "blue", {76, 108, 171} },
    { "aqua", {115, 110, 153} },
    { "yellow", {139, 152, 50} },
};

cv::Vec3b conv(cv::Vec3b v, int code)
{
    cv::Mat m(1, 1, CV_8UC3);
    m.at<cv::Vec3b>(0) = v;
    cv::cvtColor(m, m, code);
    return m.at<cv::Vec3b>(0);
}

std::string getColor(cv::Vec3b rgb)
{
    auto ycc = conv(rgb, CV_BGR2YCrCb);
    double len2 = 100000;
    std::string dst;
    auto calcLen2 = [](cv::Vec3b v0, cv::Vec3b v1){
        double d = 0;
        for(int i = 0; i < 3; ++i){
            d += (v0[i] - v1[i]) * (v0[i] - v1[i]);
        }
        return d;
    };
    for(auto c : colors){
        auto len = calcLen2(ycc, c.ycc);
        if(len < len2){
            len2 = len;
            dst = c.name;
        }
    }
    return dst;
}

/*!
 カメラの画像からブロックの輪郭を抽出する
 @param[in] m カメラ画像(RGB)
 @return ブロックの輪郭
 */
std::vector<cv::Point> getBlockContour(cv::Mat const & m)
{
    cv::Mat hls;
    cv::cvtColor(m, hls, CV_BGR2HLS);
    std::vector<cv::Mat> v;
    cv::split(hls, v);
    auto l = v[1];
    auto s = v[2];
    double r = 0.5;
	auto mixed = r * l + (1 - r) * s;
    cv::Mat block;
    cv::threshold(mixed, block, 80, 255, cv::THRESH_BINARY);
    typedef std::vector<cv::Point> contour_t;
    std::vector<contour_t> contours;
    cv::findContours(block, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // 一番面積の広い領域がブロックと判断する。ただし画像サイズなみの面積だった場合は除外
    contour_t cach;
    double maxArea = -1;
    for(auto points:contours){
        double area = cv::contourArea(points);
        if(maxArea < area && area < m.cols * m.rows * 0.9){
            maxArea = area;
            cach = points;
        }
    }
    return cach;
}

/*!
 テスト画像を作る。
 カメラがなくても開発をするため。
 @param[in] rows ブロック段数
 @return テスト画像
 */
cv::Mat createTestImage(int rows)
{
    cv::Mat dst = cv::Mat::zeros(CAMERA_WIDTH * IMAGE_RATIO, CAMERA_HEIGHT * IMAGE_RATIO, CV_8UC3);
    for(int row = 0; row < rows; ++row){
        int y = dst.rows - BLOCK_SIZE * (row + 2);
        int x = dst.cols / 2 - BLOCK_SIZE + (rand() % BLOCK_SIZE);
        cv::Rect rc(x, y, BLOCK_SIZE, BLOCK_SIZE);
        auto ycc = colors[rand() % 6].ycc;
        auto bgr = conv(ycc, CV_YCrCb2BGR);
        cv::Scalar s(bgr[0], bgr[1], bgr[2]);
        cv::rectangle(dst, rc, s, CV_FILLED);
    }
	for (int i = 0; i < dst.size().area() / 100; ++i){
		cv::Point pt = { rand() % dst.cols, rand() % dst.rows };
		dst.at<cv::Vec3b>(pt) = { (uchar)(rand() % 256), (uchar)(rand() % 256), (uchar)(rand() % 256) };
	}
    return dst;
}

/*!
 top: second
 bottom: first
 */
std::pair<cv::Point, cv::Point> getTopBottom(std::vector<cv::Point> const & points)
{
    // top: second  bottom: first
    std::pair<cv::Point, cv::Point> dst;
    dst.first.y = -1;
    dst.second.y = 0xFFFF;
    for(auto pt : points){
        if(dst.first.y < pt.y) dst.first = pt;
        if(pt.y < dst.second.y) dst.second = pt;
    }
    return dst;
}

/*!
カメラ画像取得後のメイン処理
デバッグの都合でメイン関数に書くのやめた
@param[in] m
*/
void proc(cv::Mat m)
{
	assert(3 == m.channels());
	auto blockContour = getBlockContour(m);
	for (size_t i = 0; i < blockContour.size(); ++i){
		auto pt0 = blockContour[i];
		auto pt1 = blockContour[(i + 1) % blockContour.size()];
		cv::line(m, pt0, pt1, cv::Scalar(0, 255, 0), 4);
	}
    {
        auto tb = getTopBottom(blockContour);
        int blockCount = (tb.first.y - tb.second.y) / BLOCK_SIZE;
        std::cout << blockCount << std::endl;
        for(int i = 0; i <= blockCount; ++i){
            int y = (tb.second.y * i + tb.first.y * (blockCount - i)) / blockCount;
            cv::Point pt0(0, y);
            cv::Point pt1(m.cols, y);
            cv::line(m, pt0, pt1, cv::Scalar(0, 0, 0xFF), 2);
        }
    }
	cv::imshow("block contour", m);
    {
        auto bin = [&](){
            cv::Mat bin = cv::Mat::zeros(m.size(), CV_8UC1);
            std::vector<std::vector<cv::Point>> contours = { blockContour };
            cv::drawContours(bin, contours, 0, 255, CV_FILLED);
            return bin;
        }();
        cv::imshow("block", bin);
    }
	{
		std::cout << "[colors]" << std::endl;
		int x = m.cols / 2;
		for (int block = 0; block < 11; ++block){
			int y = m.rows * block / 11 + 50;
			auto v = m.at<cv::Vec3b>(y, x);
			std::cout << getColor(v) << std::endl;
		}
	}
}

int main(int argc, const char * argv[]) {
#ifdef _DEBUG
	srand(0);
	for(;;){
		cv::Mat m = createTestImage(1 + (rand() % 11));
		proc(m);
		cv::waitKey();
	}
#else
    cv::VideoCapture cap(1);
    if(!cap.isOpened()){
        std::cerr << "failed to open camera device." << std::endl;
        return -1;
    }
    // CV_CAP_PROP_GAIN
    cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
    while(1){
        cv::Mat m;
        cap >> m;
		cv::resize(m, m, cv::Size(), IMAGE_RATIO, IMAGE_RATIO);
		cv::flip(m.t(), m, 0);
		proc(m);
		cv::waitKey(1);
	}
#endif // _DEBUG
    return 0;
}
