#pragma once
#include "Accuracy.h"
#include "Global.h"
#include "MidiHeroSettings.h"
#include "Scoring.h"

class TimedMidiMessage :
    public enable_shared_from_this<TimedMidiMessage>
#ifdef UNIT_TESTS
    // , CtorLogger
#endif
{
public:
    MidiMessage message;
    shared_ptr<AudioPlayHead::PositionInfo> position;
    shared_ptr<MidiHeroSettings> settings;

    TimedMidiMessage(
        MidiMessage&& message,
        const shared_ptr<AudioPlayHead::PositionInfo>& position,
        const shared_ptr<MidiHeroSettings>& settings,
        const double sampleRate
    );

    double getPosition() const;

    std::string getPositionFormatted() const;

    double getIntendedPosition() const;

    std::string getIntendedPositionFormatted() const;

    Accuracy getAccuracy();
    double getPpqDiff() const;
    int getPpqDiffInMs() const;

    Scoring getScore() const;

private:
    double sampleRate;
    Accuracy accuracy;

    static double roundToDecimals(const double value, const int decimals);

    static std::string formatPPQ(double ppqValue, const AudioPlayHead::TimeSignature& signature);
};