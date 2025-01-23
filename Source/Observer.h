#pragma once
#include "Global.h"

template<typename Type>
class Observer :
    public Value::Listener
{
public:
    Observer()
    {
    }

    Observer(const function<void(const Type&)>& fn)
    {
        setCallback(fn);
    }

    void setCallback(const function<void(const Type&)>& fn)
    {
        handler = fn;
    }

private:
    void valueChanged(Value& value) override
    {
        if (handler != nullptr)
        {
            handler(value.getValue());
        }
    }

    function<void(const Type&)> handler;
};