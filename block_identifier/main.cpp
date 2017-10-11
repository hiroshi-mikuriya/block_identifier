#include "identify.h"
#include "sender.h"
#include "define.h"
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <mutex>
#include <thread>

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
	メイン処理
	@param[in] opt オプション
	@param[in] device_id カメラデバイスID
	@param[in] address PythonプロセスのIPアドレス
	@param[in] port Pythonプロセスのポート番号
	*/
	int main_proc(Option const & opt, int device_id, std::string const & address, int port)
	{
		std::vector<BlockInfo> blockInfo;
		std::mutex mutex;
		if(!address.empty()){
			std::thread th([&]{
				for (;;){
					std::cout << "Type any KEY and ENTER." << std::endl;
					std::string tmp;
					std::cin >> tmp;
					std::vector<BlockInfo> copy;
					{
						std::unique_lock<std::mutex> lock(mutex);
						copy = blockInfo;
					}
					sendTcp(opt, copy, address, port);
				}
			});
			th.detach();
		}
#ifdef _DEBUG
		srand(0);
		for (;;){
			cv::Mat m = createTestImage(1 + (rand() % 11), opt.colors);
			{
				std::unique_lock<std::mutex> lock(mutex);
				identifyBlock(m, opt.colors, blockInfo);
			}
			cv::waitKey();
		}
#else
		cv::VideoCapture cap(device_id);
		if (!cap.isOpened()){
			std::cerr << "failed to open camera device." << std::endl;
			return -1;
		}
		// CV_CAP_PROP_GAIN
		cap.set(CV_CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
		while (1){
			{
				std::unique_lock<std::mutex> lock(mutex);
				cv::Mat m;
				cap >> m;
				cv::resize(m, m, cv::Size(), IMAGE_RATIO, IMAGE_RATIO);
				cv::flip(m.t(), m, 0);
				identifyBlock(m, opt.colors, blockInfo);
			}
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
			("help,h", "show help")
			("version,v", "print sarry lib version")
			("generate,g", "Generate option file")
			("option,o", po::value<std::string>(), "Option file path")
			("device,d", po::value<int>()->default_value(0), "Camera device number if PC has multiple camera devices")
			("address,a", po::value<std::string>(), "Python process IP address")
			("port,p", po::value<int>()->default_value(80), "Python process port number");
		;
		po::variables_map vm;
		try{
			po::store(po::parse_command_line(argc, argv, desc), vm);
			if (vm.count("help")){
				std::cout << desc << std::endl;
				return 0;
			}
			if (vm.count("version")){
				std::cout << "version: 0.9.0" << std::endl;
				return 0;
			}
			if (vm.count("generate")){
				std::cout << "generating..." << std::endl;
				auto path = "block_identifier.xml";
				writeOption(path, getDefaultOption());
				std::cout << "generated file " << path << std::endl;
				return 0;
			}
			po::notify(vm);
			auto opt = getDefaultOption();
			if (vm.count("option")){
				auto path = vm["option"].as<std::string>();
				opt = readOption(path);
			}
			auto camera = vm["device"].as<int>();
			auto address = vm.count("address") ? vm["address"].as<std::string>(): "";
			auto port = vm["port"].as<int>();
			main_proc(opt, camera, address, port);
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
