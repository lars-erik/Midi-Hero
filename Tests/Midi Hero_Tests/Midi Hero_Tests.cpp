#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests.v.10.13.0.hpp>

#include "catch_amalgamated.hpp"
#include "resource.h"
#include "TestData.h"

#include <numeric>

using namespace std;

bool approx(double a, double b, double tolerance = 1e-6) {
    return fabs(a - b) <= tolerance;
}

template <typename Predicate>
vector<TimedMidiMessage> filterMessages(vector<TimedMidiMessage> messages, Predicate predicate)
{
    vector<TimedMidiMessage> notes;
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
    stream << setw(width) << (isNumber ? right: left) << value << ' ';
}

string buildReport(vector<TimedMidiMessage>& notes, int divisionLevel)
{
    ostringstream stream;

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
        AudioPlayHead::TimeSignature& timeSignature = *note.position.getTimeSignature();
        printColumn(stream, *note.position.getPpqPositionOfLastBarStart(), 10);
        printColumn(stream, note.getPosition(), 10);
        printColumn(stream, note.getPositionFormatted(divisionLevel), 10, false);
        printColumn(stream, note.getPpqDiffInMs(divisionLevel), 10);
        printColumn(stream, note.getIntendedPosition(divisionLevel), 10);
        printColumn(stream, note.getIntendedPositionFormatted(divisionLevel), 10, false);
        printColumn(stream, MidiTable::getDataString(note.message), 20, false);
        printColumn(stream, note.getScore(divisionLevel), 10);
        printColumn(stream, note.getScoreName(divisionLevel), 10, false);
        stream << endl;
    }

    return stream.str();
}

TEST_CASE("Note calculations for quantized play")
{
    constexpr int DivisionLevel = 8;
    auto model = getTestData(IDR_QUANTIZED_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });

    string report = buildReport(notes, DivisionLevel);

    ApprovalTests::Approvals::verify(report);
}

TEST_CASE("Quantized score is 100%")
{
    constexpr int DivisionLevel = 8;
    auto model = getTestData(IDR_QUANTIZED_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });

    vector<double> scores;
    transform(notes.begin(), notes.end(), back_inserter(scores), [](const TimedMidiMessage& m) { return m.getScore(DivisionLevel); });
    double score = accumulate(scores.begin(), scores.end(), 0.0);
    double totalScore = score / notes.size();

    REQUIRE(totalScore == 1);
}

TEST_CASE("Note calculations for off play")
{
    constexpr int DivisionLevel = 4;

    auto model = getTestData(IDR_OFF_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });

    string report = buildReport(notes, DivisionLevel);

    ApprovalTests::Approvals::verify(report);
}

TEST_CASE("Off score is 76%")
{
    constexpr int DivisionLevel = 4;
    auto model = MidiListModel(getTestData(IDR_OFF_CSV_FILE));

    auto score = model.score(DivisionLevel);

    INFO("Testing appx " << score.total << " ~ " << .76);
    REQUIRE(approx(score.total, .76));
}