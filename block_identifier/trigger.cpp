#include "trigger.h"
#include <iostream>
#include <boost/format.hpp>

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

#ifdef ENABLE_RASPBERRY_PI_CAMERA
#include <bcm2835.h>
class ButtonTrigger : public Trigger
{
    int m_gpio;
    int sampling(int count)
    {
        int c = 0;
        for(int i = 0; i < count; ++i){
            c += bcm2835_gpio_lev(m_gpio);
        }
        return c;
    }
public:
    ButtonTrigger()
    : m_gpio(24)
    {
        if(!bcm2835_init()){
            std::cerr << "failed to init bcm2835" << std::endl;
            exit(1);
        }
        bcm2835_gpio_fsel(m_gpio, 0 );
        bcm2835_gpio_set_pud(m_gpio, 1);
    }
    void wait()
    {
        int const count = 20;
        while(sampling(count) != 0){
            bcm2835_delay(1);
        }
        while(sampling(count) != count){
            bcm2835_delay(1);
        }
    }
};
#endif // ENABLE_RASPBERRY_PI_CAMERA

std::shared_ptr<Trigger> Trigger::create()
{
#ifdef ENABLE_RASPBERRY_PI_CAMERA
    return std::make_shared<ButtonTrigger>();
#else
    return std::make_shared<StdinTrigger>();
#endif
}
