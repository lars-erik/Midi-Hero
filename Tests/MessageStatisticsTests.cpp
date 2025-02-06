#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>
#include "catch2/catch_all.hpp"

#include <numeric>

#include "TestData.h"
#include "TestUtils.h"

using namespace Catch::Matchers;

struct StatisticsFixture
{
    MidiListModel model;
    shared_ptr<MidiHeroSettings> settings;

    StatisticsFixture(int divisionLevel, const string& csvData) :
        model(getTestData(csvData)),
        settings(model.getSettings())
    {
        settings->setDivisionLevel(divisionLevel);
        settings->getTiming().setScale(1.0);

        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
    }

    string buildReport() const
    {
        int divisionLevel = model.getSettings()->getDivisionLevel();
        auto summary = ::buildStatisticsReport(model);
        auto details = ::buildReport(model.getNotes(), divisionLevel);

        return summary + details;
    }
};

struct QuantizedFixture : StatisticsFixture
{
    QuantizedFixture() : StatisticsFixture(16, quantizedCsv)
    {
    }
};

TEST_CASE_METHOD(QuantizedFixture, "Quantized scoring")
{
    SECTION("reports")
    {
        string report = buildReport();
        ApprovalTests::Approvals::verify(report);
    }

    SECTION("has score 100%")
    {
        auto theModel = model;
        auto score = model.getScore();
        REQUIRE_THAT(score.average, WithinAbs(1, .001f));
    }
}

struct OffFixture : StatisticsFixture
{
    OffFixture() : StatisticsFixture(4, offCsv)
    {
    }
};

TEST_CASE_METHOD(OffFixture, "Off scoring")
{

    SECTION("reports")
    {
        string report = buildReport();
        ApprovalTests::Approvals::verify(report);
    }

    SECTION("has unscaled score 76%")
    {
        auto score = model.getScore();

        vector<TimedMidiMessage> notes;
        auto notePtrs = model.getNotes();
        transform(begin(notePtrs), end(notePtrs), back_inserter(notes), [](shared_ptr<TimedMidiMessage> const& m) { return *m; });

        REQUIRE_THAT(score.average, WithinAbs(.76f, .001f));
    }

    SECTION("has scaled to .5 score 50%")
    {
        model.getSettings()->getTiming().setScale(.5f);
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);

        auto score = model.getScore();
        REQUIRE_THAT(score.average, WithinAbs(.5f, .001f));
    }
}

