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

std::vector<TestData::Block> const Exp2 = {
    { "white", 2 },
    { "white", 3 },
    { "white", 4 },
    { "brown", 2 },
    { "brown", 2 },
    { "brown", 2 },
    { "brown", 3 },
    { "brown", 3 },
    { "brown", 3 },
    { "brown", 4 },
    { "brown", 4 },
    { "brown", 4 },
    { "red", 2 },
    { "red", 2 },
    { "red", 3 },
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
    { "Thu Jun  7 203901 2018.png", Exp2 },
    { "Thu Jun  7 203902 2018.png", Exp2 },
    { "Thu Jun  7 203903 2018.png", Exp2 },
    { "Thu Jun  7 203904 2018.png", Exp2 },
    { "Thu Jun  7 203905 2018.png", Exp2 },
    { "Thu Jun  7 203906 2018.png", Exp2 },
    { "Thu Jun  7 203907 2018.png", Exp2 },
    { "Thu Jun  7 203908 2018.png", Exp2 },
    { "Thu Jun  7 203909 2018.png", Exp2 },
    { "Thu Jun  7 203910 2018.png", Exp2 },
    { "Thu Jun  7 203911 2018.png", Exp2 },
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
