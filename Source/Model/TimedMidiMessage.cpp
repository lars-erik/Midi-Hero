#include "TimedMidiMessage.h"

TimedMidiMessage::TimedMidiMessage(
    MidiMessage&& message,
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

double TimedMidiMessage::getPosition() const
{
    double samplesPerPpq = sampleRate * 60 / position->getBpm().orFallback(60);
    double adjustedPpqPosition = message.getTimeStamp() / samplesPerPpq + position->getPpqPosition().orFallback(0);
    double result = roundToDecimals(adjustedPpqPosition, 3);
    return result;
}

std::string TimedMidiMessage::getPositionFormatted() const
{
    return formatPPQ(getPosition(), *position->getTimeSignature());
}

double TimedMidiMessage::getIntendedPosition() const {
    double step = 1.0 / settings->getDivisionLevel();
    double nearest = std::round(getPosition() / step) * step;
    return nearest;
}

std::string TimedMidiMessage::getIntendedPositionFormatted() const
{
    return formatPPQ(getIntendedPosition(), *position->getTimeSignature());
}

double TimedMidiMessage::getPpqDiff() const
{
    return getPosition() - getIntendedPosition();
}

int TimedMidiMessage::getPpqDiffInMs() const
{
    double secPerQuarterNote = 60 / position->getBpm().orFallback(60); // 0.5 in 120
    double diff = getPpqDiff(); // ratio of 0.5? (yes)
    double diffInSeconds = diff * secPerQuarterNote;
    int result = static_cast<int>(round(diffInSeconds * 1000));
    return result;
}

Scoring TimedMidiMessage::getScore() const
{
    if (!message.isNoteOn())
    {
        String exMsg = "Cannot score " + message.getDescription();
        throw runtime_error(exMsg.toStdString());
    }

    auto* timing = &settings->getTiming();
    const int diff = abs(getPpqDiffInMs());
    if (diff < timing->getScaledPerfectMs()) return Scoring::scores[0];
    if (diff < timing->getScaledGreatMs()) return Scoring::scores[1];
    if (diff < timing->getScaledGoodMs()) return Scoring::scores[2];
    if (diff < timing->getScaledOffMs()) return Scoring::scores[3];
    return Scoring::scores[4];
}

double TimedMidiMessage::roundToDecimals(const double value, const int decimals) {
    double scale = pow(10.0, decimals);
    return round(value * scale) / scale;
}

std::string TimedMidiMessage::formatPPQ(double ppqValue, const AudioPlayHead::TimeSignature& signature) {
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