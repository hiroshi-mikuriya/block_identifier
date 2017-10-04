#include <iostream>
#include <opencv2/opencv.hpp>
#include <cassert>

/// カメラ横サイズ
const int CAMERA_WIDTH = 1280;
/// カメラ縦サイズ
const int CAMERA_HEIGHT = 720;
/// 画像縮尺率
const double IMAGE_RATIO = 0.5;
/// IMAGE_RATIOをかけたあとのブロックサイズ（高さ）
const int BLOCK_SIZE = static_cast<int>(102 * IMAGE_RATIO);
/// IMAGE_RATIOをかけたあとのブロックサイズ（幅）
const int BLOCK_SIZE_WIDTH = static_cast<int>(150 * IMAGE_RATIO);

/*!
 色情報
 */
struct Color
{
    std::string name; ///< 色名
    cv::Vec3b bgr; ///< RGB値
};

/*!
 プログラムが扱う色種
 */
Color const Colors[] = {
    { "red", {0x80, 0x80, 0xFF} },
    { "green", {0x80, 0xFF, 0x80} },
    { "white", {0xFC, 0xFC, 0xFC} },
    { "blue", {0xFF, 0x80, 0x80} },
    { "aqua", {0xFF, 0xE0, 0xC0} },
    { "yellow", {0x80, 0xFF, 0xFF} },
};

/*!
 Vec3bの色空間を変換する
 例：BGR -> YCrCb : conv(v, CV_BGR2YCrCb);
 @param[in] v 変換対象の色
 @param[in] code 変換方法
 @return 変換後の色
 */
cv::Vec3b conv(cv::Vec3b v, int code)
{
    cv::Mat m(1, 1, CV_8UC3);
    m.at<cv::Vec3b>(0) = v;
    cv::cvtColor(m, m, code);
    return m.at<cv::Vec3b>(0);
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
        for(int i = 0; i < 3; ++i){
            d += (v0[i] - v1[i]) * (v0[i] - v1[i]);
        }
        return d;
    };
    for(auto c : Colors){
        auto len = calcLen2(bgr, c.bgr);
        if(len < len2){
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
std::vector<cv::Point> getBlockContour(cv::Mat const & m)
{
    cv::Mat hls;
    cv::cvtColor(m, hls, CV_BGR2HLS);
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
    dst += cv::Scalar::all(10);
    int SIZES[] = { 1, 1, 1, 1, 2, 2, 3 };
    for(int row = 0; row < rows; ++row){
        int y = dst.rows - BLOCK_SIZE * (row + 2);
        int x = dst.cols / 2 - (rand() % BLOCK_SIZE) / 2;
        int type = SIZES[rand() % 7];
        cv::Rect rc(x, y, BLOCK_SIZE_WIDTH * type, BLOCK_SIZE);
        auto bgr = Colors[rand() % 6].bgr;
        cv::Scalar s(bgr[0], bgr[1], bgr[2]);
        cv::rectangle(dst, rc, s, CV_FILLED);
        if(rows - 1 <= row){
            int b = rc.height / 5;
            for(int i = 0; i < type; ++i){
                cv::rectangle(dst, cv::Rect(rc.x + BLOCK_SIZE_WIDTH * i + b, rc.y - b, b, b), s, CV_FILLED);
                cv::rectangle(dst, cv::Rect(rc.x + BLOCK_SIZE_WIDTH * (i + 1) - 2 * b, rc.y - b, b, b), s, CV_FILLED);
            }
        }
    }
    for (int i = 0; i < dst.size().area() / 50; ++i){
        cv::Point pt = { rand() % dst.cols, rand() % dst.rows };
        dst.at<cv::Vec3b>(pt) = { (uchar)(rand() % 256), (uchar)(rand() % 256), (uchar)(rand() % 256) };
    }
    return dst;
}

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
    for(dst.top = 0; m.data[dst.top] < th; dst.top++);
    for(dst.bottom = m.cols - 1; m.data[dst.bottom] < th; dst.bottom--);
    return dst;
}

/*!
 ブロック情報
 */
struct BlockInfo
{
    Color color; ///< ブロックの色
    cv::Rect rc; ///< ブロックの矩形
    cv::Rect color_area; ///< ブロック色判定領域
    cv::Vec3b ave; ///< 平均色
    int type; ///< 横幅: 1, 2, 3
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
                    static_cast<int>(rc.width * r),
                    static_cast<int>(rc.height * r)
                    );
}

/*!
 画像内の指定した矩形の全ブロック情報を取得する
 @param[in] m 画像
 @param[in] points ブロックの矩形
 @return ブロック情報
 */
std::vector<BlockInfo> getBlockInfo(cv::Mat const & m, std::vector<cv::Point> const & points)
{
    auto const bin = [&m, &points](){
        cv::Mat bin = cv::Mat::zeros(m.size(), CV_8UC1);
        std::vector<std::vector<cv::Point>> contours = { points };
        cv::drawContours(bin, contours, 0, 255, CV_FILLED);
        return bin;
    }();
    cv::imshow("bin", bin);
    auto getAveBgr = [](cv::Mat const & src){
        cv::Mat tmp;
        cv::reduce(src, tmp, 1, CV_REDUCE_AVG);
        cv::reduce(tmp, tmp, 0, CV_REDUCE_AVG); // TODO: クラッシュすることがある。原因不明
        return tmp.at<cv::Vec3b>(0);
    };
    auto tb = getTopBottom(bin);
    std::vector<BlockInfo> dst;
    if(tb.bottom <= tb.top){
        return dst;
    }
    int blockCount = (tb.bottom - tb.top + BLOCK_SIZE / 2) / BLOCK_SIZE;
    for (int i = 0; i < blockCount; ++i){
        int y = (tb.top * (blockCount - i) + tb.bottom * i) / blockCount;
        cv::Mat ary;
        cv::reduce(bin(cv::Rect(0, y, m.cols, BLOCK_SIZE)), ary, 0, CV_REDUCE_AVG);
        int left = 0;
        const int th = 220;
        for (; ary.data[left] < th && left < ary.cols; ++left);
        int right = bin.cols - 1;
        for (; ary.data[right] < th && 0 <= right; --right);
        if (right <= left) continue; // 計算できなかったので仕方ないからあきらめる
        BlockInfo info;
        info.rc = cv::Rect(left, y, right - left, BLOCK_SIZE);
        info.color_area = info.rc * 0.2;
        info.ave = getAveBgr(m(info.color_area));
        info.color = getColor(info.ave);
        info.type = (right - left + BLOCK_SIZE_WIDTH / 2) / BLOCK_SIZE_WIDTH;
        dst.push_back(info);
    }
    return dst;
}

/*!
 カメラ画像取得後のメイン処理
 デバッグの都合でメイン関数に書くのやめた
 @param[in] m カメラ画像 or デバッグ画像
 */
void proc(cv::Mat const & m)
{
    assert(3 == m.channels());
    auto const contour = getBlockContour(m);
    auto const blockInfo = getBlockInfo(m, contour);
    cv::Mat canvas = cv::Mat::zeros(m.rows, m.cols * 2, CV_8UC3);
    m.copyTo(canvas(cv::Rect(0, 0, m.cols, m.rows)));
    for (auto info : blockInfo){
        cv::rectangle(canvas, info.rc, cv::Scalar(0, 255, 0), 1);
        cv::rectangle(canvas, info.color_area, cv::Scalar(255, 0, 255), 1);
        auto v = info.color.bgr;
        std::stringstream ss;
        ss << info.type << ":" << info.color.name << " " << std::hex << (int)info.ave[2] << (int)info.ave[1] << (int)info.ave[0];
        cv::putText(canvas, ss.str(), cv::Point(m.cols * 1.1, info.rc.y + info.rc.height * 0.7), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(v[0], v[1], v[2]));
    }
    cv::imshow("blocks", canvas);
}

/*!
 main proc
 @param[in] argc arguments count
 @param[in] argv arguments
 @return exit code
 */
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

