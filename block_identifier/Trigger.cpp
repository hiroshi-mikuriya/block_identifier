#include "Trigger.h"
#if defined _WIN32 || defined _WIN64
#include "Serial.h"
#endif // defined _WIN32 || defined _WIN64
#include <iostream>
#include <boost/format.hpp>

#if defined _WIN32 || defined _WIN64
class ArduinoButton : public Trigger
{
    Serial m;
public:
    ArduinoButton(int com)
    {
        PortInfo info;
        info.m_port = com;
        info.m_baudrate = 9600;
        info.m_bytesize = 8;
        info.m_parity = 0;
        info.m_stopbits = 1;
        std::cout << boost::format("Opening COM Port%d ...") % com << std::endl;
        m.Open(info);
    }
    void Wait()
    {
        std::cout << "PUSH BUTTON !" << std::endl;
        std::vector<unsigned char> tmp;
        while (!m.Receive(tmp));
    }
};
#endif // defined _WIN32 || defined _WIN64

class StdinTrigger : public Trigger
{
public:
    StdinTrigger()
    {
    }
    void Wait()
    {
        std::cout << "Input any key and Enter..." << std::endl;
        std::string tmp;
        std::cin >> tmp;
    }
};

std::shared_ptr<Trigger> Trigger::getTrigger(int com)
{
#if defined _WIN32 || defined _WIN64
    if (0 < com){
        return std::make_shared<ArduinoButton>(com);
    }
#endif // defined _WIN32 || defined _WIN64
    return std::make_shared<StdinTrigger>();
}
