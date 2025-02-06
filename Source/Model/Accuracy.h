#pragma once
#include "Global.h"
#include "MidiHeroSettings.h"

class TimedMidiMessage;

class Accuracy
{
public:
    Accuracy() = default;
    Accuracy(shared_ptr<const TimedMidiMessage> m);
    Accuracy(int ms, shared_ptr<MidiHeroSettings> const& settings);

    bool isEmpty() const;
    string getDescription() const;
    string toString() const;

    friend ostream& operator<<(ostream& os, const Accuracy& acc);

private:
    int getMs() const;

    shared_ptr<const TimedMidiMessage> message = nullptr;
    shared_ptr<MidiHeroSettings> settings = nullptr;
    int ms = 0;
};

