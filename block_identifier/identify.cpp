#include "identify.h"
#include <boost/format.hpp>

#ifdef _DEBUG
#define DEBUG_SHOW(s, m)  cv::imshow((s), (m))
#else
#define DEBUG_SHOW(s, m)
#endif

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
    
    /**
     * 線分を引くアニメーションを描画する
     */
    void drawLineAnnimation(std::string windowName,
                            cv::Mat & canvas,
                            cv::Point pt0,
                            cv::Point pt1,
                            cv::Scalar color,
                            int thickness,
                            int speed)
    {
        double len = [pt0, pt1](){
            int x = pt0.x - pt1.x;
            int y = pt0.y - pt1.y;
            return std::sqrt(x * x + y * y);
        }();
        auto inner = [](cv::Point pt0, double d0, cv::Point pt1, double d1)->cv::Point{
            int x = static_cast<int>(pt0.x * d0 + pt1.x * d1) / (d0 + d1);
            int y = static_cast<int>(pt0.y * d0 + pt1.y * d1) / (d0 + d1);
            return { x, y };
        };
        auto start = pt0;
        for(int i = 1; i <= len; i += speed){
            auto end = inner(pt0, len - i, pt1, i);
            cv::line(canvas, start, end, color, thickness);
            start = end;
            cv::imshow(windowName, canvas);
            cv::waitKey(1);
        }
        cv::line(canvas, start, pt1, color, thickness);
        cv::imshow(windowName, canvas);
        cv::waitKey(1);
    }

    /**
     * 輪郭を引くアニメーションを描画する
     */
    void drawContourAnnimation(std::string windowName,
                               cv::Mat & canvas,
                               std::vector<cv::Point> const & contour,
                               cv::Scalar color,
                               int thickness)
    {
        double len = cv::arcLength(contour, true);
        int speed = static_cast<int>(std::max(1.0, len / 100));
        for(size_t i = 0; i < contour.size(); ++i){
            auto pt0 = contour[i];
            auto pt1 = contour[(i + 1) % contour.size()];
            drawLineAnnimation(windowName, canvas, pt0, pt1, color, thickness, speed);
        }
    }
    
    void drawRectAnnimation(std::string windowName,
                            cv::Mat & canvas,
                            cv::Rect const & rc,
                            cv::Scalar color,
                            int thickness)
    {
        std::vector<cv::Point> pts = { { rc.x, rc.y }, { rc.x + rc.width, rc.y }, { rc.x + rc.width, rc.y + rc.height }, { rc.x, rc.y + rc.height } };
        drawContourAnnimation(windowName, canvas, pts, color, thickness);
    }
    
    void showAnnimation(std::string windowName,
                        cv::Mat const & img,
                        std::vector<cv::Point> const & contour,
                        std::vector<BlockInfo> const & blockInfo)
    {
        cv::Mat canvas = img.clone();
        auto simpleContour = contour;
        cv::approxPolyDP(contour, simpleContour, 10, true);
        drawContourAnnimation(windowName, canvas, simpleContour, cv::Scalar(0, 255, 0), 5);
        for(int i = 0; i < 256; i += 4){
            cv::Mat mask(canvas.size(), CV_8UC3);
            mask = cv::Scalar::all(255 - i);
            std::vector<std::vector<cv::Point>> contours = { simpleContour };
            cv::drawContours(mask, contours, 0, cv::Scalar::all(255), CV_FILLED);
            canvas &= mask;
            cv::imshow(windowName, canvas);
            cv::waitKey(1);
        }
        for(auto bi : blockInfo) {
            drawRectAnnimation(windowName, canvas, bi.rc, cv::Scalar(255, 0, 0), 3);
        }
        cv::waitKey();
    }

    /*!
    画像からレゴブロックを認識するクラス
    */
    class IdentifyBlock
    {
        IdentifyBlock & operator = (const IdentifyBlock &) = delete;

        cv::Mat const image_;
        Option const opt_;

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
            for (dst.top = 0; m.data[dst.top] < opt_.tune.stud_th; dst.top++);
            for (dst.bottom = m.cols - 1; m.data[dst.bottom] < opt_.tune.stud_th; dst.bottom--);
            return dst;
        }

        /*!
        ブロック情報の画像を表示する
        @param[in] blockInfo ブロック情報
        */
        void showBlockInfo(std::vector<BlockInfo> const & blockInfo)
        {
            auto to_instname = [this](Block const & block){
                auto inst = opt_.block2inst.find(block);
                return inst == opt_.block2inst.end() ? "unknown" : inst->second.name;
            };
            cv::Mat canvas = cv::Mat::zeros(image_.rows, image_.cols * 2, CV_8UC3);
            image_.copyTo(canvas(cv::Rect(0, 0, image_.cols, image_.rows)));
            for (auto info : blockInfo){
                cv::rectangle(canvas, info.rc, cv::Scalar(0, 255, 0), 1);
                cv::rectangle(canvas, info.color_area, cv::Scalar(255, 0, 255), 1);
                auto instname = to_instname(info.to_block());
                auto v = info.color.bgr;
                auto f = boost::format("%d:%s %02X %02X %02X") % info.width % info.color.name % (int)info.ave[2] % (int)info.ave[1] % (int)info.ave[0];
                cv::putText(canvas, f.str() + "(" + instname + ")", cv::Point2f(image_.cols * 1.1f, info.rc.y + info.rc.height * 0.4f), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(v[0], v[1], v[2]));
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
            for (auto c : opt_.colors){
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
        @return ブロックの輪郭
        */
        std::vector<cv::Point> getBlockContour()
        {
            cv::Mat hsv;
            cv::cvtColor(image_, hsv, CV_BGR2HSV);
            std::vector<cv::Mat> splits;
            cv::split(hsv, splits);
            auto s = splits[1];
            auto v = splits[2];
            auto mixed = s + v;
            DEBUG_SHOW("HSV[H]", splits[0]);
            DEBUG_SHOW("HSV[S]", s);
            DEBUG_SHOW("HSV[V]", v);
            DEBUG_SHOW("mixed", mixed);
            cv::Mat block;
            cv::threshold(mixed, block, opt_.tune.bin_th, 255, cv::THRESH_BINARY);
            DEBUG_SHOW("block", block);
            {
                // ブロックは継ぎ目で上下に途切れることがあるので、上下に拡大して結合する。
                int const iteration = 5;
                cv::Mat kernel(3, 1, CV_8UC1);
                kernel = 255;
                cv::dilate(block, block, kernel, cv::Point(-1, -1), iteration);
                cv::erode(block, block, kernel, cv::Point(-1, -1), iteration);
            }
            DEBUG_SHOW("block dilate erode", block);
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
            /*! 背景：黒　輪郭内：黒　の画像を作る*/
            auto const bin = [this, &points](){
                cv::Mat bin = cv::Mat::zeros(image_.size(), CV_8UC1);
                std::vector<std::vector<cv::Point>> contours = { points };
                cv::drawContours(bin, contours, 0, 255, CV_FILLED);
                return bin;
            }();
            /*! 画像の平均色を返す */
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
            int blockCount = (tb.bottom - tb.top + opt_.tune.get_block_height() / 2) / opt_.tune.get_block_height();
            for (int i = 0; i < blockCount; ++i){
                int y = (tb.top * (blockCount - i) + tb.bottom * i) / blockCount;
                if (bin.rows - opt_.tune.get_block_height() < y){
                    continue;
                }
                cv::Mat ary;
                cv::reduce(bin(cv::Rect(0, y, bin.cols, opt_.tune.get_block_height())), ary, 0, CV_REDUCE_AVG);
                int left = 0;
                for (; ary.data[left] < opt_.tune.size_th && left < ary.cols; ++left);
                int right = bin.cols - 1;
                for (; ary.data[right] < opt_.tune.size_th && 0 <= right; --right);
                if (right <= left) continue; // 計算できなかったので仕方ないからあきらめる
                BlockInfo info;
                info.rc = cv::Rect(left, y, right - left, opt_.tune.get_block_height());
                info.color_area = info.rc * 0.2;
                info.ave = getAveBgr(image_(info.color_area));
                info.color = getColor(info.ave);
                info.width = (right - left + opt_.tune.get_block_width() / 2) / opt_.tune.get_block_width();
                dst.push_back(info);
            }
            return dst;
        }
    public:
        IdentifyBlock(
            cv::Mat const & image,
            Option const & opt,
            std::vector<BlockInfo> & blockInfo)
            : image_(image)
            , opt_(opt)
        {
            assert(3 == image_.channels());
            auto const contour = getBlockContour();
            blockInfo = getBlockInfo(contour);
            showAnnimation("annimation", image, contour, blockInfo);
            showBlockInfo(blockInfo);
        }
    };
}

void identifyBlock(
    cv::Mat const & image,
    Option const & opt,
    std::vector<BlockInfo> & blockInfo)
{
    IdentifyBlock(image, opt, blockInfo);
}
