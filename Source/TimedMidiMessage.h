#pragma once
#include "Global.h"

struct TimedMidiMessage
#ifdef UNIT_TESTS
    // : CtorLogger
#endif
{
    MidiMessage message;
    shared_ptr<AudioPlayHead::PositionInfo> position;
    double sampleRate;

    TimedMidiMessage(
        const MidiMessage&& message,
        const shared_ptr<AudioPlayHead::PositionInfo>& position,
        const shared_ptr<MidiHeroSettings>& settings,
        const double sampleRate
    ) :
        message(std::move(message)),
        position(position),
        settings(settings),
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

    int getPpqDiffInMs() const
    {
        const int divisionLevel = settings->getDivisionLevel();
        double secPerQuarterNote = 60 / position->getBpm().orFallback(60); // 0.5 in 120
        double diff = getPpqDiff(divisionLevel); // ratio of 0.5? (yes)
        double diffInSeconds = diff * secPerQuarterNote;
        int result = static_cast<int>(round(diffInSeconds * 1000));
        return result;
    }

    double getScore() const
    {
        auto* timing = &settings->getTiming();
        const int diff = abs(getPpqDiffInMs());
        if (diff < timing->getScaledPerfectMs()) return 1;
        if (diff < timing->getScaledGreatMs()) return .9;
        if (diff < timing->getScaledGoodMs()) return .6;
        if (diff < timing->getScaledOffMs()) return .25;
        return 0;
    }

    std::string getScoreName() const
    {
        auto* timing = &settings->getTiming();
        const int diff = abs(getPpqDiffInMs());
        if (diff < timing->getScaledPerfectMs()) return "Perfect";
        if (diff < timing->getScaledGreatMs()) return "Great";
        if (diff < timing->getScaledGoodMs()) return "Good";
        if (diff < timing->getScaledOffMs()) return "Off";
        return "Bad";
    }

private:
    shared_ptr<MidiHeroSettings> settings;

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