#include "Trigger.h"
#include "Serial.h"
#include <iostream>
#include <boost/format.hpp>

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
        std::vector<unsigned char> buf;
        while (!m.Receive(buf));
    }
};

std::shared_ptr<Trigger> Trigger::getTrigger(int com)
{
    // TODO: COM=0‚È‚ç•W€“ü—Í‚É‚·‚é
    return std::make_shared<ArduinoButton>(com);
}
