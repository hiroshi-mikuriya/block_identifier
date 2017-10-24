#pragma once

#include <memory>

class Trigger
{
public:
    /*!
    トリガーを取得する
    */
    static std::shared_ptr<Trigger> getTrigger(int com);
    /*!
    トリガーが発生するまで待つ
    */
    virtual void Wait() = 0;
};
