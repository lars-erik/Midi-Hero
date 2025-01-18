#include "JuceHeader.h"
#include "catch_amalgamated.hpp"
#include "resource.h"
#include "TestData.h"

using namespace juce;

std::vector<TimedMidiMessage> getTestData(int identifier)
{
    auto midiData = readCsvFile(identifier, L"CSV");
    auto model = transformToModel(midiData, 120, 44100);
    return model;
}

bool approx(double a, double b, double tolerance = 1e-6) {
    return std::fabs(a - b) <= tolerance;
}

template <typename Predicate>
std::vector<TimedMidiMessage> filterMessages(std::vector<TimedMidiMessage> messages, Predicate predicate)
{
    std::vector<TimedMidiMessage> notes;
    std::copy_if(
        messages.begin(),
        messages.end(),
        std::back_inserter(notes),
        predicate
    );
    return notes;
}

TEST_CASE("Can read MIDI data")
{
    auto model = getTestData(IDR_QUANTIZED_CSV_FILE);

    REQUIRE(model.size() == 35);
    REQUIRE(model.at(0).message.isMidiStart());
}

TEST_CASE("Calculates PPQ for quantized notes")
{
    auto model = getTestData(IDR_QUANTIZED_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
    auto verifyPosition = [notes](int index, double expected)
    {
        double actual = notes.at(index).getAdjustedPpqPosition();
        double result = approx(actual, expected);
        INFO("Note at " << index << " expected at " << expected << " and is at " << actual << " which is " << (result ? "correct" : "false"));
        REQUIRE(result);
        
    };

    REQUIRE(notes.size() == 16);
    verifyPosition(0, 0);
    verifyPosition(1, 1);
    verifyPosition(2, 2);
    verifyPosition(3, 3);
    verifyPosition(4, 4);
    verifyPosition(5, 4.5);
    verifyPosition(6, 5);
    verifyPosition(7, 5.5);
    verifyPosition(8, 6);
    verifyPosition(9, 6.25);
    verifyPosition(10, 6.5);
    verifyPosition(11, 6.75);
    verifyPosition(12, 7);
    verifyPosition(13, 7.125);
    verifyPosition(14, 7.25);
    verifyPosition(15, 7.375);
}

TEST_CASE("Predicts intended PPQ position for non quantized input")
{
    auto model = getTestData(IDR_OFF_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
    auto verifyIntendedPosition = [notes](int index, double expected)
    {
        double actual = notes.at(index).getIntendedPpqPosition(4);
        double result = approx(actual, expected);
        INFO("Note intention at " << index << " expected at " << expected << " and is at " << actual << " which is " << (result ? "correct" : "false") << ". It is at PPQ " << notes.at(index).getAdjustedPpqPosition());
        REQUIRE(result);
    };

    REQUIRE(notes.size() == 13);
    verifyIntendedPosition(0, 0.25); // This is actually very off. 
    verifyIntendedPosition(1, 1);
    verifyIntendedPosition(2, 2);
    verifyIntendedPosition(3, 3);
    verifyIntendedPosition(4, 4);
    verifyIntendedPosition(5, 4.5);
    verifyIntendedPosition(6, 5);
    verifyIntendedPosition(7, 5.5);
    verifyIntendedPosition(8, 6);
    verifyIntendedPosition(9, 6.25);
    verifyIntendedPosition(10, 6.5);
    verifyIntendedPosition(11, 6.75);
}

TEST_CASE("Calculates diff in PPQ between intended position and actual position")
{
    constexpr int DivisionLevel = 4;

    auto model = getTestData(IDR_OFF_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
    auto verifyDiff = [notes](int index, double expected)
    {
        double actual = notes.at(index).getPpqDiff(DivisionLevel);
        double result = approx(actual, expected);
        INFO("Note diff at " << index << " expected at " << expected << " and is at " << actual << " which is " 
            << (result ? "correct" : "false") 
            << ". It is at PPQ " << notes.at(index).getAdjustedPpqPosition()
            << " and expected at " << notes.at(index).getIntendedPpqPosition(4));
        REQUIRE(result);
    };

    REQUIRE(notes.size() == 13);
    verifyDiff(0, -0.09);
    verifyDiff(1, 0.022);
    verifyDiff(2, 0.014);
    verifyDiff(3, -0.042);
    verifyDiff(4, 0.038);
    verifyDiff(5, -0.053);
    verifyDiff(6, .068);
    verifyDiff(7, -0.02);
    verifyDiff(8, 0.014);
    verifyDiff(9, 0.033);
    verifyDiff(10, 0.06);
    verifyDiff(11, 0.077);
    verifyDiff(12, 0.001);
}

TEST_CASE("Calculates diff in ms between intended position and actual position")
{
    constexpr int DivisionLevel = 4;

    auto model = getTestData(IDR_OFF_CSV_FILE);
    auto notes = filterMessages(model, [&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
    auto verifyDiff = [notes](int index, double expected)
        {
            int actual = notes.at(index).getPpqDiffInMs(DivisionLevel);
            int result = actual == expected;
            INFO("Note diff at " << index << " expected at " << expected << " and is at " << actual << " which is "
                << (result ? "correct" : "false")
                << ". It is at PPQ " << notes.at(index).getAdjustedPpqPosition()
                << " and expected at " << notes.at(index).getIntendedPpqPosition(4));
            REQUIRE(result);
        };

    for(int i = 0; i<notes.size(); i++)
    {
        std::cout << i << ": " << notes[i].getPpqDiffInMs(DivisionLevel) << std::endl;
    }

    REQUIRE(notes.size() == 13);
    verifyDiff(0, -0.09);
    verifyDiff(1, 0.022);
    verifyDiff(2, 0.014);
    verifyDiff(3, -0.042);
    verifyDiff(4, 0.038);
    verifyDiff(5, -0.053);
    verifyDiff(6, .068);
    verifyDiff(7, -0.02);
    verifyDiff(8, 0.014);
    verifyDiff(9, 0.033);
    verifyDiff(10, 0.06);
    verifyDiff(11, 0.077);
    verifyDiff(12, 0.001);
}
