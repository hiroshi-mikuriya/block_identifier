#include "identify.h"
#include "sender.h"
#include "trigger.h"
#include "mycamera.hpp"
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <thread>
#include <chrono>
#include <ctime>

#ifdef ENABLE_RASPBERRY_PI_CAMERA
#define PLAY_SOUND(wav) system("aplay " wav);
#else
#define PLAY_SOUND(wav)
#endif

namespace {
    void save_image(cv::Mat const & m) {
        auto const now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::string name = std::ctime(&time);
        name.pop_back();
        cv::imwrite(name + ".png", m);
    }
    bool is_atari(std::vector<BlockInfo> const & infos){
        for (auto const & info : infos){
            if (info.width == 2){ // オブジェクトブロックが１つでもあればOK
                return true;
            }
        }
        return false;
    }
    /*!
    メイン処理
    @param[in] opt オプション
    @param[in] address PythonプロセスのIPアドレス
    @param[in] port Pythonプロセスのポート番号
    @param[in] save カメラ画像保存
    @return Exit code
    */
    int main_proc(Option const & opt, std::string const & address, int port, bool save)
    {
        bool triggered = false;
        std::thread th([&]{
            auto trigger = Trigger::create();
            for (;;){
                trigger->wait();
                triggered = true;
            }
        });
        MyCamera cap(0);
        if (!cap.isOpened()){
            std::cerr << "failed to open camera device." << std::endl;
            return -1;
        }
        // CV_CAP_PROP_GAIN
        cap.set(CV_CAP_PROP_FRAME_WIDTH, opt.tune.camera_width);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, opt.tune.camera_height);
        for (;;){
            cv::Mat m;
            cap >> m;
            if (m.empty()){
                std::cerr << "failed to get camera image." << std::endl;
                cv::waitKey(100);
                continue;
            }
            cv::resize(m, m, cv::Size(), opt.tune.camera_ratio, opt.tune.camera_ratio);
            cv::flip(m, m, -1);
            m = m(cv::Rect(m.cols / 3, 0, m.cols / 3, m.rows));
            if(save){
                save_image(m);
            }
            std::vector<BlockInfo> blockInfo;
            identifyBlock(m, opt, blockInfo);
            showBlocks(m, blockInfo);
            cv::waitKey(1);
            if(triggered){
                triggered = false;
                if (is_atari(blockInfo)){
                    PLAY_SOUND("../sound/atari.wav");
                    sendToServer(opt, blockInfo, address, port);
                }
                else{
                    PLAY_SOUND("../sound/hazure.wav");
                }
            }
        }
        // unreachable code.
        th.join();
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
            ("help,h", "Show help")
            ("version,v", "Print software version")
            ("generate,g", "Generate option file")
            ("option,o", po::value<std::string>(), "Option file path")
            ("address,a", po::value<std::string>(), "Python process IP address")
            ("port,p", po::value<int>()->default_value(80), "Python process port number")
            ("save,s", "Save camera images")
        ;
        po::variables_map vm;
        try{
            po::store(po::parse_command_line(argc, argv, desc), vm);
            if (vm.count("help")){
                std::cout << desc << std::endl;
                return 0;
            }
            if (vm.count("version")){
                std::cout << "version: 2.0.0" << std::endl;
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
            auto const opt = vm.count("option") ? readOption(vm["option"].as<std::string>()) : getDefaultOption();
            std::string address = vm.count("address") ? vm["address"].as<std::string>() : "";
            int port = vm["port"].as<int>();
            bool save = 0 < vm.count("save");
            return main_proc(opt, address, port, save);
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
