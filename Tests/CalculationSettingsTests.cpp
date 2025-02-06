#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>
#include "catch2/catch_all.hpp"

#include <numeric>

#include "TestData.h"
#include "TestUtils.h"

using namespace Catch::Matchers;

struct SettingsFixture
{
public:
    SettingsFixture() :
        settingsPtr(createDefaultSettings()),
        timing(&settingsPtr->getTiming())
    {  }

private:
    shared_ptr<MidiHeroSettings> settingsPtr;
public:
    MidiHeroSettings::TimingSettings* timing;
};

TEST_CASE_METHOD(SettingsFixture, "Settings have the scored millisecond intervals")
{
    SECTION("Unscaled")
    {
        array expected = { 10, 20, 40, 80 };
        array actual{
            timing->getPerfectMs(),
            timing->getGreatMs(),
            timing->getGoodMs(),
            timing->getOffMs()
        };

        REQUIRE_THAT(actual, RangeEquals(expected));
    }

    SECTION("Scaled by half")
    {
        timing->setScale(.5);

        array expected = { 5, 10, 20, 40 };
        array actual{
            timing->getScaledPerfectMs(),
            timing->getScaledGreatMs(),
            timing->getScaledGoodMs(),
            timing->getScaledOffMs()
        };

        REQUIRE_THAT(actual, RangeEquals(expected));
    }
}