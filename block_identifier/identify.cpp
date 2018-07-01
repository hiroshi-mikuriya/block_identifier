#include "identify.h"
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <boost/optional.hpp>

#ifdef _DEBUG
#define DEBUG_SHOW(s, m)  cv::imshow((s), (m))
#else
#define DEBUG_SHOW(s, m)
#endif

namespace {
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
         輪郭2値画像の両端を返す
         @param[in] bin 輪郭2値画像
         @return 両端
         */
        boost::array<int, 2> getBothEnds(cv::Mat const & bin, int th)
        {
            cv::Mat m;
            cv::reduce(bin, m, 0, CV_REDUCE_AVG);
            boost::array<int, 2> dst;
            for (dst[0] = 0; m.data[dst[0]] < th; dst[0]++);
            for (dst[1] = m.cols - 1; m.data[dst[1]] < th; dst[1]--);
            return dst;
        }
        /*!
         HSV色値から色の種類を判別する.
         @param[in] hsv HSV値
         @return 最も近い色
         */
        std::string identifyColor(cv::Vec3b hsv)
        {
            if(hsv[1] < 0x40) return "white"; // Approximately 0x05
            if(hsv[0] < 0x07) return "brown"; // Approximately 0x04
            if(hsv[0] < 0x11) return "orange"; // Approximately 0x0B
            if(hsv[0] < 0x1E) return "yellow"; // Approximately 0x1A
            if(hsv[0] < 0x40) return "yellowgreen"; // Approximately 0x23
            if(hsv[0] < 0x50) return "green"; // Approximately 0x45
            if(hsv[0] < 0x80) return "blue"; // Approximately 0x6A
            return "red"; // Approximately 0xB1
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
            auto s = splits[1]; // 彩度の画像には余計な部分も多く含まれる
            auto v = splits[2]; // 明るさの画像は彩度と違い正確
            cv::Mat tmp;
            cv::threshold(cv::min(s, v), tmp, 80, 255, cv::THRESH_BINARY); // 彩度と明るさの最小値をとることで彩度の高い部分が正確に抽出される。彩度は明るさと比べ値が小さいので、次の行で明るさと比較するため二値化する。
            auto mixed = cv::max(v, tmp); // 彩度と明るさの最大値をとることで白を抽出する。なお、白以外はtmpの時点で全て抽出されている。
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
            return largestContour(contours);
        }
        /**
        最も面積が広い輪郭を返す.
        ただし輪郭の面積が画像サイズ並みだった場合は除外する.
        */
        contour_t largestContour(std::vector<contour_t> const & contours)
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
        /*! 画像の平均色を返す */
        boost::array<cv::Vec3b, 2> calcAvgColor(cv::Mat const & src)
        {
            cv::Mat tmp;
            cv::reduce(src, tmp, 1, CV_REDUCE_AVG);
            cv::reduce(tmp, tmp, 0, CV_REDUCE_AVG);
            boost::array<cv::Vec3b, 2> dst;
            dst[0] = tmp.at<cv::Vec3b>(0);
            cv::cvtColor(tmp, tmp, CV_BGR2HSV);
            dst[1] = tmp.at<cv::Vec3b>(0);
            return dst;
        }

        boost::optional<BlockInfo> getBlockInfo(int y, cv::Mat const & bin)
        {
            cv::Mat const line = bin(cv::Rect(0, y, bin.cols, opt_.tune.get_block_height()));
            auto const lr = getBothEnds(line, opt_.tune.size_th);
            int const right = lr[1];
            int const left = lr[0];
            if (right <= left){
                return boost::none; // 計算できなかったので仕方ないからあきらめる
            }
            BlockInfo info;
            info.rc = cv::Rect(left, y, right - left, opt_.tune.get_block_height());
            info.color_area = info.rc * 0.2;
            auto const bgr_hsv = calcAvgColor(image_(info.color_area));
            info.bgr = bgr_hsv[0];
            info.hsv = bgr_hsv[1];
#if 0
            info.color = identifyColor2(info.bgr);
#else
            info.color.name = identifyColor(info.hsv);
            info.color.bgr = info.bgr;
#endif
            info.width = (right - left + opt_.tune.get_block_width() / 2) / opt_.tune.get_block_width();
            return info;
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
                std::vector<contour_t> contours = { points };
                cv::drawContours(bin, contours, 0, 255, CV_FILLED);
                return bin;
            }();
            DEBUG_SHOW("original", image_);
            DEBUG_SHOW("bin", bin);
            auto tb = getBothEnds(bin.t(), opt_.tune.stud_th);
            std::vector<BlockInfo> dst;
            if (tb[1] <= tb[0]){
                return dst;
            }
            int blockCount = (tb[1] - tb[0] + opt_.tune.get_block_height() / 2) / opt_.tune.get_block_height();
            for (int i = 0; i < blockCount; ++i){
                int y = (tb[0] * (blockCount - i) + tb[1] * i) / blockCount;
                if (bin.rows - opt_.tune.get_block_height() < y){
                    continue;
                }
                auto info = getBlockInfo(y, bin);
                if(info){
                    dst.push_back(info.get());
                }
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
        auto f = boost::format("%d:%-12s R:%02X G:%02X B:%02X H:%02X S:%02X V:%02X")
            % info.width % info.color.name
            % (int)info.bgr[2] % (int)info.bgr[1] % (int)info.bgr[0]
            % (int)info.hsv[0] % (int)info.hsv[1] % (int)info.hsv[2];
        cv::Point2f const pt(image.cols * 1.1f, info.rc.y + info.rc.height * 0.4f);
        cv::putText(canvas, f.str(), pt, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(v[0], v[1], v[2]));
    }
    cv::imshow("blocks", canvas);
}
