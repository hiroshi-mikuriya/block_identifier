#pragma once

#include <memory>

class Trigger
{
public:
    /*!
    �g���K�[���擾����
    */
    static std::shared_ptr<Trigger> getTrigger(int com);
    /*!
    �g���K�[����������܂ő҂�
    */
    virtual void Wait() = 0;
};
