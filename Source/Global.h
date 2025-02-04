// ReSharper disable CppClangTidyClangDiagnosticHeaderHygiene
#pragma once

#include "JuceHeader.h"

using namespace juce;
using namespace std;

#define READONLY_OBSERVABLE(type, name) \
private:\
    Value name;\
\
public:\
    type get##name() {\
        return static_cast<type>(name.getValue());\
    }\
\
   void observe##name(Observer<type>* observer, function<void(const type)> callback)\
   {\
       observer->setCallback(callback);\
       name.addListener(observer);\
   }\
\
   void stopObserve##name(Observer<type>* observer)\
   {\
       name.removeListener(observer);\
   }\


#define OBSERVABLE(type, name) \
    READONLY_OBSERVABLE(type, name)\
    void set##name(type value) {\
        name.setValue(value);\
    }\
\

#ifdef UNIT_TESTS
struct CtorLogger
{
    CtorLogger() {
        cout << "Default ctor" << endl;
    };

    ~CtorLogger() {
        cout << "Default dtor" << endl;
    };

    CtorLogger(const CtorLogger& other)
    {
        cout << "Copy ctor" << endl;
    }

    CtorLogger(CtorLogger&& other) noexcept
    {
        cout << "Move ctor" << endl;
    }

    CtorLogger& operator=(const CtorLogger& other)
    {
        cout << "Copy assign" << endl;
        if (this == &other)
            return *this;
        return *this;
    }

    CtorLogger& operator=(CtorLogger&& other) noexcept
    {
        cout << "Move assign" << endl;
        if (this == &other)
            return *this;
        return *this;
    }
};
#endif