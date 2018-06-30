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

    /*!
    画像からレゴブロックを認識するクラス
    */
    class IdentifyBlock
    {
        IdentifyBlock & operator = (const IdentifyBlock &) = delete;

        typedef std::vector<cv::Point> contour_t;

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
        このプログラムが認識する色の中で最も近い色を返す
        @param[in] bgr BGR値
        @return 最も近い色
        */
        Color identifyColor(cv::Vec3b bgr)
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
         HSV色値から色の種類を判別する.
         @param[in] hsv HSV値
         @return 最も近い色
         */
        std::string identifyColor2(cv::Vec3b hsv)
        {
            if(hsv[1] < 0x40) return "white"; // 実際のところ0x10未満
            if(hsv[0] < 0x07) return "brown";
            if(hsv[0] < 0x11) return "orange";
            if(hsv[0] < 0x1E) return "yellow";
            if(hsv[0] < 0x40) return "yellowgreen";
            if(hsv[0] < 0x50) return "green";
            if(hsv[0] < 0x80) return "blue";
            return "red"; // 0xB0くらい
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
            cv::Mat tmp;
            cv::threshold(cv::min(s, v), tmp, 80, 255, cv::THRESH_BINARY);
            auto mixed = cv::max(v, tmp);
            DEBUG_SHOW("mixed", mixed);
            cv::Mat block;
            cv::threshold(mixed, block, opt_.tune.bin_th, 255, cv::THRESH_BINARY);
            DEBUG_SHOW("block", block);
            {
                // ブロックは継ぎ目で上下に途切れることがあるので、上下方向に拡大/縮小して結合する。
                int const iteration = 5;
                cv::Mat kernel(3, 1, CV_8UC1);
                kernel = 255;
                cv::dilate(block, block, kernel, cv::Point(-1, -1), iteration);
                cv::erode(block, block, kernel, cv::Point(-1, -1), iteration);
            }
            DEBUG_SHOW("block dilate erode", block);
            std::vector<contour_t> contours;
            cv::findContours(block, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
            return getBiggestAreaContour(contours);
        }
        /**
        最も面積が広い輪郭を返す.
        ただし輪郭の面積が画像サイズ並みだった場合は除外する.
        */
        contour_t getBiggestAreaContour(std::vector<contour_t> const & contours)
        {
            contour_t res;
            double maxArea = -1;
            for (auto points : contours){
                double area = cv::contourArea(points);
                if (maxArea < area && area < image_.size().area() * 0.9){
                    maxArea = area;
                    res = points;
                }
            }
            return res;
        }
        /*!
        画像内の指定した矩形の全ブロック情報を取得する
        @param[in] points ブロックの矩形
        @return ブロック情報
        */
        std::vector<BlockInfo> getBlockInfo(std::vector<cv::Point> const & points)
        {
            /*! 背景：黒　輪郭内：白　の画像を作る*/
            auto bin = [this, &points](){
                cv::Mat bin = cv::Mat::zeros(image_.size(), CV_8UC1);
                std::vector<std::vector<cv::Point>> contours = { points };
                cv::drawContours(bin, contours, 0, 255, CV_FILLED);
                return bin;
            }();
            DEBUG_SHOW("original", image_);
            DEBUG_SHOW("bin", bin);

            /*! 画像の平均色を返す */
            auto calcAveBgr = [](cv::Mat const & src){
                cv::Mat tmp;
                cv::reduce(src, tmp, 1, CV_REDUCE_AVG);
                cv::reduce(tmp, tmp, 0, CV_REDUCE_AVG);
                std::vector<cv::Vec3b> dst(2);
                dst[0] = tmp.at<cv::Vec3b>(0);
                cv::cvtColor(tmp, tmp, CV_BGR2HSV);
                dst[1] = tmp.at<cv::Vec3b>(0);
                return dst;
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
                auto avg = calcAveBgr(image_(info.color_area));
                info.bgr = avg[0];
                info.hsv = avg[1];
#if 0
                info.color = identifyColor(info.bgr);
#else
                info.color.name = identifyColor2(info.hsv);
                info.color.bgr = info.bgr;
#endif
                info.width = (right - left + opt_.tune.get_block_width() / 2) / opt_.tune.get_block_width();
                dst.push_back(info);
            }
            for(auto block : dst){
                cv::rectangle(bin, block.rc, 128);
            }
            DEBUG_SHOW("block rects", bin);
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

void showBlocks(
    cv::Mat const & image,
    std::vector<BlockInfo> const & blockInfo)
{
    cv::Mat canvas = cv::Mat::zeros(image.rows, image.cols + 640, CV_8UC3);
    image.copyTo(canvas(cv::Rect(0, 0, image.cols, image.rows)));
    for (auto info : blockInfo){
        cv::rectangle(canvas, info.rc, cv::Scalar(0, 255, 0), 1);
        cv::rectangle(canvas, info.color_area, cv::Scalar(255, 0, 255), 1);
        auto v = info.color.bgr;
        auto f = boost::format("%d:%-6s <RGB> %02X %02X %02X <HSV> %02X %02X %02X") % info.width % info.color.name % (int)info.bgr[2] % (int)info.bgr[1] % (int)info.bgr[0] % (int)info.hsv[0] % (int)info.hsv[1] % (int)info.hsv[2];
        cv::putText(canvas, f.str(), cv::Point2f(image.cols * 1.1f, info.rc.y + info.rc.height * 0.4f), cv::FONT_HERSHEY_DUPLEX, 0.7, cv::Scalar(v[0], v[1], v[2]));
    }
    cv::imshow("blocks", canvas);
}
