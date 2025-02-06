#pragma once
#include "JuceHeader.h"
#include "MidiTable.h"
using namespace juce;

struct PositionInfoBuilder
{
    AudioPlayHead::PositionInfo positionInfo;

    PositionInfoBuilder& withBpm(double bpm)
    {
        positionInfo.setBpm(bpm);
        return *this;
    }

    PositionInfoBuilder& withIsPlaying(bool isPlaying) {
        positionInfo.setIsPlaying(isPlaying);
        return *this;
    }

    PositionInfoBuilder& withTimeInSeconds(double timeInSeconds) {
        positionInfo.setTimeInSeconds(timeInSeconds);
        return *this;
    }

    AudioPlayHead::PositionInfo build() const {
        return positionInfo;
    }
};

inline bool approx(double a, double b, double tolerance = 1e-6) {
    return fabs(a - b) <= tolerance;
}

template <typename Predicate>
vector<shared_ptr<TimedMidiMessage>> filterMessages(vector<shared_ptr<TimedMidiMessage>> messages, Predicate predicate)
{
    vector<shared_ptr<TimedMidiMessage>> notes;
    copy_if(
        messages.begin(),
        messages.end(),
        back_inserter(notes),
        predicate
    );
    return notes;
}

template<typename T>
void printColumn(ostringstream& stream, T value, int width, bool isNumber = true) {
    stream << setw(width) << (isNumber ? right : left) << value << ' ';
}

inline shared_ptr<MidiHeroSettings> createDefaultSettings()
{
    ValueTree tree{ "root" };
    auto settings = make_shared<MidiHeroSettings>(tree);
    return settings;
}

inline string buildReport(vector<shared_ptr<TimedMidiMessage>> const& notes, int const divisionLevel)
{
    ostringstream stream;

    stream << "Details" << endl << endl;

    printColumn(stream, "Bar", 10, false);
    printColumn(stream, "PPQ", 10, false);
    printColumn(stream, "Position", 10, false);
    printColumn(stream, "MS Offset", 10, false);
    printColumn(stream, "Int. PPQ", 10, false);
    printColumn(stream, "Intended", 10, false);
    printColumn(stream, "Data", 20, false);
    printColumn(stream, "Score", 10, false);
    printColumn(stream, "Judgement", 10, false);
    stream << endl;

    for (auto& note : notes)
    {
        AudioPlayHead::TimeSignature& timeSignature = *note->position->getTimeSignature();
        printColumn(stream, *note->position->getPpqPositionOfLastBarStart(), 10);
        printColumn(stream, note->getPosition(), 10);
        printColumn(stream, note->getPositionFormatted(), 10, false);
        printColumn(stream, note->getPpqDiffInMs(), 10);
        printColumn(stream, note->getIntendedPosition(), 10);
        printColumn(stream, note->getIntendedPositionFormatted(), 10, false);
        printColumn(stream, MidiTable::getDataString(note->message), 20, false);
        printColumn(stream, note->getScore().score, 10);
        printColumn(stream, note->getScore().getScoreName(), 10, false);
        stream << endl;
    }

    stream << endl;

    return stream.str();
}

inline string buildScoreCountReport(MidiListModel const& model)
{
    auto scoreCounts = model.getScoreCounts();
    ostringstream stream;

    stream << "Summary" << endl << endl;

    stream << "Total:   " << (model.getScore().average * 100) << "% - " << model.getScore().getScoreName() << endl << endl;

    stream << "Perfect: " << scoreCounts["Perfect"] << endl;
    stream << "Great:   " << scoreCounts["Great"] << endl;
    stream << "Good:    " << scoreCounts["Good"] << endl;
    stream << "Off:     " << scoreCounts["Off"] << endl;
    stream << "Bad:     " << scoreCounts["Bad"] << endl;

    stream << endl;

    return stream.str();
}