#include "JuceHeader.h"
#include "catch_amalgamated.hpp"
#include "resource.h"
#include "TestData.h"

using namespace juce;

std::vector<TimedMidiMessage> getTestData()
{
    auto midiData = readCsvFile(IDR_CSV_FILE, L"CSV");
    auto model = transformToModel(midiData, 120, 44100);
    return model;
}

TEST_CASE("Can read MIDI data", "[basic]")
{
    auto model = getTestData();

    REQUIRE(model.size() == 35);
    REQUIRE(model.at(0).message.isMidiStart());
}

TEST_CASE("Calculates PPQ for quantized notes", "[basic]")
{
    auto model = getTestData();
    std::vector<TimedMidiMessage> notes;
    std::copy_if(
        model.begin(), 
        model.end(), 
        std::back_inserter(notes), 
        [&](const TimedMidiMessage& msg)
        {
            return msg.message.isNoteOn();
        }
    );

    REQUIRE(notes.size() == 16);
    REQUIRE(notes.at(0).getAdjustedPpqPosition() == 0);
    REQUIRE(notes.at(1).getAdjustedPpqPosition() == 1);
    REQUIRE(notes.at(2).getAdjustedPpqPosition() == 2);
    REQUIRE(notes.at(3).getAdjustedPpqPosition() == 3);
    REQUIRE(notes.at(4).getAdjustedPpqPosition() == 4);
    REQUIRE(notes.at(5).getAdjustedPpqPosition() == 4.5);
    REQUIRE(notes.at(6).getAdjustedPpqPosition() == 5);
    REQUIRE(notes.at(7).getAdjustedPpqPosition() == 5.5);
    REQUIRE(notes.at(8).getAdjustedPpqPosition() == 6);
    REQUIRE(notes.at(9).getAdjustedPpqPosition() == 6.25);
    REQUIRE(notes.at(10).getAdjustedPpqPosition() == 6.5);
    REQUIRE(notes.at(11).getAdjustedPpqPosition() == 6.75);
    REQUIRE(notes.at(12).getAdjustedPpqPosition() == 7);
    REQUIRE(notes.at(13).getAdjustedPpqPosition() == 7.125);
    REQUIRE(notes.at(14).getAdjustedPpqPosition() == 7.25);
    REQUIRE(notes.at(15).getAdjustedPpqPosition() == 7.375);
}

bool approx(double a, double b, double tolerance = 1e-6) {
    return std::fabs(a - b) <= tolerance;
}

double predictClosestPosition(double inputPosition, int divisionLevel) {
    double step = 1.0 / divisionLevel; // Grid step size
    double nearest = std::round(inputPosition / step) * step;
    return nearest;
}

TEST_CASE("Quantization Prediction") {
    REQUIRE(approx(predictClosestPosition(0.49, 2), 0.5));  // 8ths
    REQUIRE(approx(predictClosestPosition(0.26, 4), 0.25)); // 16ths
    REQUIRE(approx(predictClosestPosition(0.13, 8), 0.125)); // 32nds
    REQUIRE(approx(predictClosestPosition(0.87, 4), 0.75)); // 16ths
    REQUIRE(approx(predictClosestPosition(1.01, 2), 1.0));  // Out of range
}