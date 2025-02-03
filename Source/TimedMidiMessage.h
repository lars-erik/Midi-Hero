#pragma once
#include "Global.h"

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

struct TimedMidiMessage
#ifdef UNIT_TESTS
    : CtorLogger
#endif
{
    MidiMessage message;
    shared_ptr<AudioPlayHead::PositionInfo> position;
    double sampleRate;

    TimedMidiMessage() = default;

    // TODO: Pass reference to settings?
    TimedMidiMessage(
        const MidiMessage&& message,
        const shared_ptr<AudioPlayHead::PositionInfo>& position,
        const double sampleRate
    ) :
        message(std::move(message)),
        position(position),
        sampleRate(sampleRate)
    {
    }

    double getPosition() const
    {
        double samplesPerPpq = sampleRate * 60 / position->getBpm().orFallback(60);
        double adjustedPpqPosition = message.getTimeStamp() / samplesPerPpq + position->getPpqPosition().orFallback(0);
        double result = roundToDecimals(adjustedPpqPosition, 3);
        return result;
    }

    std::string getPositionFormatted() const
    {
        return formatPPQ(getPosition(), *position->getTimeSignature());
    }

    double getIntendedPosition(const int divisionLevel) const {
        double step = 1.0 / divisionLevel; // Grid step size
        double nearest = std::round(getPosition() / step) * step;
        return nearest;
    }

    std::string getIntendedPositionFormatted(const int divisionLevel) const
    {
        return formatPPQ(getIntendedPosition(divisionLevel), *position->getTimeSignature());
    }

    double getPpqDiff(const int divisionLevel) const
    {
        return getPosition() - getIntendedPosition(divisionLevel);
    }

    int getPpqDiffInMs(const int divisionLevel) const
    {
        double secPerQuarterNote = 60 / position->getBpm().orFallback(60); // 0.5 in 120
        double diff = getPpqDiff(divisionLevel); // ratio of 0.5? (yes)
        double diffInSeconds = diff * secPerQuarterNote;
        int result = static_cast<int>(round(diffInSeconds * 1000));
        return result;
    }

    double getScore(const int divisionLevel) const
    {
        const int diff = abs(getPpqDiffInMs(divisionLevel));
        if (diff < 10) return 1;
        if (diff < 20) return .9;
        if (diff < 40) return .6;
        if (diff < 80) return .25;
        return 0;
    }

    std::string getScoreName(const int divisionLevel) const
    {
        const int diff = abs(getPpqDiffInMs(divisionLevel));
        if (diff < 10) return "Perfect";
        if (diff < 20) return "Great";
        if (diff < 40) return "Good";
        if (diff < 80) return "Off";
        return "Bad";
    }

private:
    static double roundToDecimals(const double value, const int decimals) {
        double scale = pow(10.0, decimals);
        return round(value * scale) / scale;
    }

    static std::string formatPPQ(double ppqValue, const AudioPlayHead::TimeSignature& signature) {
        // Calculate the number of quarter notes in a bar
        double beatsPerBar = static_cast<double>(signature.numerator);
        double sixteensPerQuarter = 16.0 / signature.denominator;

        // Calculate bar, quarter, and 32nd note positions
        int bar = static_cast<int>(std::floor(ppqValue / beatsPerBar)) + 1;
        double barPosition = std::fmod(ppqValue, beatsPerBar);
        int quarter = static_cast<int>(std::floor(barPosition)) + 1;
        double fractionalQuarter = barPosition - std::floor(barPosition);
        int sixteenth = static_cast<int>(std::round(fractionalQuarter * sixteensPerQuarter)) + 1;

        // Ensure sixteenth is within valid range
        if (sixteenth > static_cast<int>(sixteensPerQuarter)) {
            sixteenth = 1;
            quarter++;
            if (quarter > signature.numerator) {
                quarter = 1;
                bar++;
            }
        }

        // Format the output as bar.quarter.sixteenth
        return std::to_string(bar) + "." + std::to_string(quarter) + "." + std::to_string(sixteenth);
    }

};