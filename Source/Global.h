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
private:\
    Value name;\
\
public:\
    type get##name() {\
        return static_cast<type>(name.getValue());\
    }\
\
    void set##name(type value) {\
        name.setValue(value);\
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

