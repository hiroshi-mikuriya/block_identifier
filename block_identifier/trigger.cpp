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

std::shared_ptr<Trigger> Trigger::create()
{
    return std::make_shared<StdinTrigger>();
}
