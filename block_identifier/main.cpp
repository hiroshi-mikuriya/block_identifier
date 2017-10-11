#include "identify.h"
#include "define.h"
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <fstream>

namespace {
    /*!
     テスト画像を作る。
     カメラがなくても開発をするため。
     @param[in] rows ブロック段数
     @return テスト画像
     */
    cv::Mat createTestImage(int rows, std::vector<Color> const & colors)
    {
        cv::Mat dst = cv::Mat::zeros(static_cast<int>(CAMERA_WIDTH * IMAGE_RATIO), static_cast<int>(CAMERA_HEIGHT * IMAGE_RATIO), CV_8UC3);
        dst += cv::Scalar::all(10);
        int SIZES[] = { 1, 1, 1, 1, 2, 2, 3 };
        for(int row = 0; row < rows; ++row){
            int y = dst.rows - BLOCK_SIZE * (row + 2);
            int x = dst.cols / 2 - (rand() % BLOCK_SIZE) / 2;
            int type = SIZES[rand() % 7];
            cv::Rect rc(x, y, BLOCK_SIZE_WIDTH * type, BLOCK_SIZE);
            auto bgr = colors[rand() % colors.size()].bgr;
            cv::Scalar s(bgr[0], bgr[1], bgr[2]);
            cv::rectangle(dst, rc, s, CV_FILLED);
            int b = rc.height / 5;
            for(int i = 0; i < type; ++i){
                cv::rectangle(dst, cv::Rect(rc.x + BLOCK_SIZE_WIDTH * i + b, rc.y - b, b, b), s, CV_FILLED);
                cv::rectangle(dst, cv::Rect(rc.x + BLOCK_SIZE_WIDTH * (i + 1) - 2 * b, rc.y - b, b, b), s, CV_FILLED);
            }
        }
        for (int i = 0; i < dst.size().area() / 50; ++i){
            cv::Point pt = { rand() % dst.cols, rand() % dst.rows };
            dst.at<cv::Vec3b>(pt) = { (uchar)(rand() % 256), (uchar)(rand() % 256), (uchar)(rand() % 256) };
        }
        return dst;
    }

	/*!
	*/
	int main_proc(Option const & opt)
	{
		std::vector<BlockInfo> blockInfo;
#ifndef _DEBUG
		srand(0);
		for (;;){
			cv::Mat m = createTestImage(1 + (rand() % 11), opt.colors);
			identifyBlock(m, opt.colors, blockInfo);
			cv::waitKey();
		}
#else
		cv::VideoCapture cap(0);
		if (!cap.isOpened()){
			std::cerr << "failed to open camera device." << std::endl;
			return -1;
		}
		// CV_CAP_PROP_GAIN
		cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
		while (1){
			cv::Mat m;
			cap >> m;
			cv::resize(m, m, cv::Size(), IMAGE_RATIO, IMAGE_RATIO);
			cv::flip(m.t(), m, 0);
			identifyBlock(m, opt.colors, blockInfo);
			cv::waitKey(1);
		}
#endif // _DEBUG
		return 0;
	}
}

/*!
 main proc
 @param[in] argc arguments count
 @param[in] argv arguments
 @return exit code
 */
int main(int argc, const char * argv[]) {
	try{
		namespace po = boost::program_options;
		po::options_description desc("options");
		desc.add_options()
			("version,v", "print sarry lib version")
			("generate,g", po::value<std::string>()->default_value("options.xml"), "Generate option file")
			("option,o", po::value<std::string>(), "Option file path")
			("camera,c", po::value<int>()->default_value(0), "Camera number if PC has multiple camera devices")
			("address,a", po::value<std::string>(), "Python process IP address")
			("port,p", po::value<int>()->default_value(80), "Python process port number");
		;
		po::variables_map vm;
		try{
			po::store(po::parse_command_line(argc, argv, desc), vm);
			if (vm.count("version")){
				std::cout << "version: 0.9.0" << std::endl;
				return 0;
			}
			if (vm.count("generate")){
				auto path = vm["generate"].as<std::string>();
				std::ofstream ofs(path);
				boost::archive::xml_oarchive oa(ofs);
				oa << boost::serialization::make_nvp("option", getDefaultOption());
				return 0;
			}
			po::notify(vm);
		}
		catch (std::exception const & e){
			std::cerr << e.what() << "\n" << desc << std::endl;
			return 1;
		}
		return 0;
	}
	catch (std::exception const & e){
		std::cerr << e.what() << std::endl;
		return 1;
	}





}

