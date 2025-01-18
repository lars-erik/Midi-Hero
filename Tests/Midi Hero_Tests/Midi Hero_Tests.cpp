#include <ApprovalTests.v.10.13.0.hpp>

#include "JuceHeader.h"
#include "catch_amalgamated.hpp"
#include "resource.h"
#include "TestData.h"

#include <numeric>

using namespace juce;
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
    stream << std::setw(width) << (isNumber ? right: left) << value << ' ';
}

string buildReport(vector<TimedMidiMessage>& notes, int divisionLevel)
{
    ostringstream stream;

    printColumn(stream, "Bar", 10, false);
    printColumn(stream, "Position", 10, false);
    printColumn(stream, "Intended", 10, false);
    printColumn(stream, "Diff", 10, false);
    printColumn(stream, "Data", 20, false);
    printColumn(stream, "Score", 10, false);
    printColumn(stream, "Judgement", 10, false);
    stream << endl;

    for (auto& note : notes)
    {
        printColumn(stream, *note.position.getPpqPositionOfLastBarStart(), 10);
        printColumn(stream, note.getAdjustedPpqPosition(), 10);
        printColumn(stream, note.getIntendedPpqPosition(divisionLevel), 10);
        printColumn(stream, note.getPpqDiffInMs(divisionLevel), 10);
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

TEST_CASE("Off score is 60%")
{
    constexpr int DivisionLevel = 4;
    auto model = getTestData(IDR_OFF_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });

    vector<double> scores;
    transform(notes.begin(), notes.end(), back_inserter(scores), [](const TimedMidiMessage& m) { return m.getScore(DivisionLevel); });
    double score = accumulate(scores.begin(), scores.end(), 0.0);
    double totalScore = round(score / notes.size() * 100) / 100;

    INFO("Has score " << score << " out of " << notes.size() << " yielding total score " << totalScore);
    REQUIRE(approx(totalScore, .6));
}