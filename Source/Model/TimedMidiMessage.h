#pragma once
#include "Global.h"
#include "MidiHeroSettings.h"
#include "Scoring.h"

struct TimedMidiMessage
#ifdef UNIT_TESTS
    // : CtorLogger
#endif
{
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

    double getPpqDiff() const;

    int getPpqDiffInMs() const;

    Scoring getScore() const;

private:
    double sampleRate;

    static double roundToDecimals(const double value, const int decimals);

    static std::string formatPPQ(double ppqValue, const AudioPlayHead::TimeSignature& signature);
};