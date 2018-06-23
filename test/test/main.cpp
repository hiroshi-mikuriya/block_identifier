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
    { "Thu Jun  7 16/57/05 2018.png", Exp },
    { "Thu Jun  7 16/57/06 2018.png", Exp },
    { "Thu Jun  7 16/57/07 2018.png", Exp },
    { "Thu Jun  7 16/57/08 2018.png", Exp },
    { "Thu Jun  7 16/57/09 2018.png", Exp },
    { "Thu Jun  7 16/57/10 2018.png", Exp },
    { "Thu Jun  7 16/57/11 2018.png", Exp },
    { "Thu Jun  7 16/57/12 2018.png", Exp },
    { "Thu Jun  7 16/57/13 2018.png", Exp },
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
        Option opt;
        std::vector<BlockInfo> blockInfo;
        identifyBlock(m, opt, blockInfo);
        cv::waitKey();
        return 0;
    }
}
