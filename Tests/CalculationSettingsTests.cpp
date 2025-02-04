#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>
#include "catch2/catch_all.hpp"

#include <numeric>

#include "TestData.h"
#include "TestUtils.h"

using namespace Catch::Matchers;

TEST_CASE("Settings have the scored millisecond intervals")
{
    auto settingsPtr = createDefaultSettings();
    MidiHeroSettings& settings = *settingsPtr;
    auto timing = settings.getTiming();

    array expected = { 10, 20, 40, 80 };
    array actual{
        timing.getPerfectMs(),
        timing.getGreatMs(),
        timing.getGoodMs(),
        timing.getOffMs()
    };

    REQUIRE_THAT(actual, RangeEquals(expected));
}

TEST_CASE("Settings may have scaled millisecond intervals")
{
    auto settingsPtr = createDefaultSettings();
    MidiHeroSettings& settings = *settingsPtr;
    auto timing = settings.getTiming();
    timing.setScale(.5);

    array expected = { 5, 10, 20, 40 };
    array actual{
        timing.getScaledPerfectMs(),
        timing.getScaledGreatMs(),
        timing.getScaledGoodMs(),
        timing.getScaledOffMs()
    };

    REQUIRE_THAT(actual, RangeEquals(expected));
}