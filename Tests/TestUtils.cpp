#include "TestUtils.h"

PositionInfoBuilder& PositionInfoBuilder::withBpm(double bpm)
{
    positionInfo.setBpm(bpm);
    return *this;
}

PositionInfoBuilder& PositionInfoBuilder::withIsPlaying(bool isPlaying) {
    positionInfo.setIsPlaying(isPlaying);
    return *this;
}

PositionInfoBuilder& PositionInfoBuilder::withTimeInSeconds(double timeInSeconds) {
    positionInfo.setTimeInSeconds(timeInSeconds);
    return *this;
}

AudioPlayHead::PositionInfo PositionInfoBuilder::build() const {
    return positionInfo;
}

bool approx(double a, double b, double tolerance = 1e-6) {
    return fabs(a - b) <= tolerance;
}

shared_ptr<MidiHeroSettings> createDefaultSettings()
{
    ValueTree tree{ "root" };
    auto settings = make_shared<MidiHeroSettings>(tree);
    return settings;
}

string buildReport(vector<shared_ptr<TimedMidiMessage>> const& notes, int const divisionLevel)
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

string formatPercent(double percent)
{
    ostringstream str;
    str << fixed << setprecision(0) << round(percent * 100) << "%";
    return str.str();
}

string buildStatisticsReport(MidiListModel const& model)
{
    auto stats = model.getStatistics();
    auto scoreCounts = stats.getCounts();
    ostringstream stream;

    stream << "Summary" << endl << endl;

    stream << "Total:           " << formatPercent(model.getScore().average) << " - " << model.getScore().getScoreName() << endl;
    stream << "Average MS diff: " << setprecision(0) << model.getStatistics().getAvgMs() << "ms" << endl;
    stream << "Max MS diff:     " << setprecision(0) << model.getStatistics().getMaxMs() << "ms" << endl;
    stream << "Min MS diff:     " << setprecision(0) << model.getStatistics().getMinMs() << "ms" << endl;
    stream << endl;

    stream << setw(10) << left << "Key" << setw(10) << "Total" << setw(10) << "Early" << setw(10) << "Perfect" << setw(10) << "Late" << endl;
    for (auto key : Scoring::keys)
    {
        stream
            << setw(10) << left << (key + ":")
            << setw(10) << (to_string(scoreCounts[key].total) + " (" + formatPercent(scoreCounts[key].ratioOf(model.getNoteCount())) + ")")
            << setw(10) << (to_string(scoreCounts[key].early) + " (" + formatPercent(scoreCounts[key].earlyRatio()) + ")")
            << setw(10) << (to_string(scoreCounts[key].exact) + " (" + formatPercent(scoreCounts[key].exactRatio()) + ")")
            << setw(10) << (to_string(scoreCounts[key].late) + " (" + formatPercent(scoreCounts[key].lateRatio()) + ")")
            << endl;
    }

    stream << endl;

    return stream.str();
}