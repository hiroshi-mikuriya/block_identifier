#include "trigger.h"
#if defined _WIN32 || defined _WIN64
#include "serial.h"
#endif // defined _WIN32 || defined _WIN64
#include <iostream>
#include <boost/format.hpp>

#if defined _WIN32 || defined _WIN64
#pragma comment(lib,"Winmm.lib")
#include <Windows.h>
#include <mmsystem.h>
class ArduinoButton : public Trigger
{
    Serial m;
public:
    ArduinoButton(int com)
    {
        PortInfo info;
        info.port = com;
        info.baudrate = 9600;
        info.bytesize = 8;
        info.parity = 0;
        info.stopbits = 1;
        std::cout << boost::format("Opening COM Port%d ...") % com << std::endl;
        m.open(info);
    }
    void wait()
    {
        std::cout << "\n" "PUSH BUTTON !" << std::endl;
        std::vector<unsigned char> tmp;
        while (tmp.size() != 1){
            m.receive(tmp); // 0D0Aも一緒に受信するときと、別々に受信するときがある
            for (;;){
                if (tmp.empty() || (tmp.back() != 0x0A && tmp.back() != 0x0D)){
                    break;
                }
                tmp.pop_back();
            }
        }
        if (!PlaySound(L"se_maoudamashii_chime13.wav", 0, SND_FILENAME | SND_SYNC | SND_NODEFAULT)){
            std::cerr << "PlaySound returns false." << std::endl;
        }
    }
};
#endif // defined _WIN32 || defined _WIN64

class StdinTrigger : public Trigger
{
public:
    StdinTrigger()
    {
    }
    void wait()
    {
        std::cout << "Input any key and Enter..." << std::endl;
        std::string tmp;
        std::cin >> tmp;
    }
};

std::shared_ptr<Trigger> Trigger::create(int com)
{
#if defined _WIN32 || defined _WIN64
    if (0 < com){
        return std::make_shared<ArduinoButton>(com);
    }
#endif // defined _WIN32 || defined _WIN64
    return std::make_shared<StdinTrigger>();
}
