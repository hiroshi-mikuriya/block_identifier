#include <opencv2/opencv.hpp>
#include "identify.h"


struct TestData
{
    std::string path;

    struct Block
    {
        std::string color_name;
        int width;
    };
    std::vector<Block> expect;
};

std::vector<TestData::Block> const Exp = {
    { "green", 2 },
    { "green", 2 },
    { "green", 2 },
    { "green", 3 },
    { "green", 3 },
    { "green", 3 },
    { "green", 4 },
    { "green", 4 },
    { "green", 4 },
    { "red", 2 },
    { "red", 2 },
    { "red", 2 },
    { "red", 3 },
    { "red", 3 },
    { "red", 4 },
    { "red", 4 },
    { "red", 4 },
};

std::vector<TestData> const Data = {
    // { "Thu Jun  7 165705 2018.png", Exp },
    { "Thu Jun  7 165706 2018.png", Exp },
    { "Thu Jun  7 165707 2018.png", Exp },
    { "Thu Jun  7 165708 2018.png", Exp },
    { "Thu Jun  7 165709 2018.png", Exp },
    { "Thu Jun  7 165710 2018.png", Exp },
    { "Thu Jun  7 165711 2018.png", Exp },
    { "Thu Jun  7 165712 2018.png", Exp },
    { "Thu Jun  7 165713 2018.png", Exp },
};

int main(int argc, const char * argv[])
{
    for(auto d : Data){
        std::cout << "test data : " << d.path << std::endl;
        std::string const path = "../../../../../../images/" + d.path;
        cv::Mat m = cv::imread(path);
        if(m.empty()){
            std::cout << "failed to load " + d.path << std::endl;
            continue;
        }
        Option opt = getDefaultOption();
        std::vector<BlockInfo> blockInfo;
        identifyBlock(m, opt, blockInfo);
        cv::waitKey();
    }
    return 0;
}
