#include <opencv2/opencv.hpp>
#include "identify.h"

std::string const Data[] = {  
    "white1.png",
    "white2.png",
    "white3.png",
    "white4.png",
    "brown1.png",
    "brown2.png",
    "brown3.png",
    "brown4.png",
    "brown5.png",
    "brown6.png",
    "brown7.png",
    "orange1.png",
    "orange2.png",
    "orange3.png",
    "orange4.png",
    "orange5.png",
    "orange6.png",
    "yellow1.png",
    "yellow2.png",
    "yellow3.png",
    "yellow4.png",
    "yellow5.png",
    "yellow6.png",
    "yellowgreen1.png",
    "yellowgreen2.png",
    "yellowgreen3.png",
    "yellowgreen4.png",
    "yellowgreen5.png",
    "yellowgreen6.png",
    "yellowgreen7.png",
    "green1.png",
    "green2.png",
    "green3.png",
    "green4.png",
    "green5.png",
    "blue1.png",
    "blue2.png",
    "blue3.png",
    "blue4.png",
    "blue5.png",
    "blue6.png",
    "red1.png",
    "red2.png",
    "red3.png",
    "red4.png",
    "red5.png",
    "red6.png",
    "red7.png",
};



int main(int argc, const char * argv[])
{
    for(auto d : Data){
        std::cout << "test data : " << d << std::endl;
        std::string const path = "../../../../../../images/" + d;
        cv::Mat m = cv::imread(path);
        if(m.empty()){
            std::cout << "failed to load " + d << std::endl;
            continue;
        }
        Option opt = getDefaultOption();
        std::vector<BlockInfo> blockInfo;
        identifyBlock(m, opt, blockInfo);
        showBlocks(m, blockInfo);
        cv::waitKey();
    }
    return 0;
}
