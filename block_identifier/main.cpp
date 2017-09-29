#include <iostream>
#include <opencv2/opencv.hpp>

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

std::string getColor(cv::Vec3b rgb)
{
    cv::Mat m(1,1,CV_8UC3);
    m.at<cv::Vec3b>(0) = rgb;
    cv::cvtColor(m, m, CV_BGR2YCrCb);
    cv::Vec3b ycc = m.at<cv::Vec3b>(0);
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

int main(int argc, const char * argv[]) {
    cv::VideoCapture cap(1);
    if(!cap.isOpened()){
        std::cerr << "failed to open camera device." << std::endl;
        return -1;
    }
    // CV_CAP_PROP_GAIN
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
    while(1){
        cv::Mat m;
        cap >> m;
        double r = 0.5;
        cv::resize(m, m, cv::Size(), r, r);
        cv::flip(m.t(), m, 0);
        cv::imshow("src", m);
        {
            cv::Mat hls;
            cv::cvtColor(m, hls, CV_BGR2HLS);
            std::vector<cv::Mat> v;
            cv::split(hls, v);
            auto l = v[1];
            auto s = v[2];
            auto res = 0.5 * l + 0.5 * s;
            cv::imshow("block", res);
            cv::Mat bin;
            cv::threshold(res, bin, 90, 255, cv::THRESH_BINARY);
            cv::imshow("block + bin", bin);
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
            bin = m.clone();
            for(auto points:contours){
                for(size_t i = 0; i < points.size(); ++i){
                    auto pt0 = points[i];
                    auto pt1 = points[(i + 1) % points.size()];
                    cv::line(bin, pt0, pt1, cv::Scalar(0, 255, 0), 4);
                }
            }
            cv::imshow("contour", bin);
        }
        {
            std::cout << "[colors]" << std::endl;
            int x = m.cols / 2;
            for(int block = 0; block < 11; ++block){
                int y = m.rows * block / 11 + 50;
                auto v = m.at<cv::Vec3b>(y, x);
                std::cout << getColor(v) << std::endl;
            }
        }
        cv::waitKey(1);
    }
    return 0;
}
