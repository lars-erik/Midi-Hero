#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>
#include "catch2/catch_all.hpp"

#include "TimedMidiMessage.h"

TEST_CASE("Score accumulation")
{
    Scoring scores []{
        { 1.0 },
        { 0.9 },
        { 0.6 },
        { 1.0 }
    };

    Scoring total = std::accumulate(begin(scores), end(scores), Scoring());

    REQUIRE(total.notes == 4);
    REQUIRE_THAT(total.score, Catch::Matchers::WithinAbs(3.5, .0001));
    REQUIRE_THAT(total.average, Catch::Matchers::WithinAbs(.88, .0001));
}