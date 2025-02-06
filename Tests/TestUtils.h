#pragma once
#include "Global.h"
#include "MidiTable.h"

struct PositionInfoBuilder
{
    AudioPlayHead::PositionInfo positionInfo;

    PositionInfoBuilder& withBpm(double bpm);

    PositionInfoBuilder& withIsPlaying(bool isPlaying);

    PositionInfoBuilder& withTimeInSeconds(double timeInSeconds);

    AudioPlayHead::PositionInfo build() const;
};

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

shared_ptr<MidiHeroSettings> createDefaultSettings();

string buildReport(vector<shared_ptr<TimedMidiMessage>> const& notes, int const divisionLevel);

string buildStatisticsReport(MidiListModel const& model);