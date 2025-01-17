#include "JuceHeader.h"
#include "catch_amalgamated.hpp"
#include "resource.h"
#include "TestData.h"

using namespace juce;

TEST_CASE("I'm alive", "[basic]")
{
    auto midiData = readCsvFile(IDR_CSV_FILE, L"CSV");
    auto first = midiData.at(0);
    auto second = midiData.at(1);

    REQUIRE(first.byte1 == 0xfa);
    REQUIRE(second.byte1 == 0xfc);
    REQUIRE(second.timeInSeconds == 1);
}

