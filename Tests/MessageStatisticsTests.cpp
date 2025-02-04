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
    vector<shared_ptr<TimedMidiMessage>> notes;

    StatisticsFixture(int divisionLevel, const string& csvData) :
        model(getTestData(csvData)),
        notes(model.getNotes())
    {
        model.getSettings()->setDivisionLevel(divisionLevel);
    }

    string buildReport() const
    {
        int divisionLevel = model.getSettings()->getDivisionLevel();
        return ::buildReport(notes, divisionLevel);
    }
};

struct QuantizedFixture : StatisticsFixture
{
    QuantizedFixture() : StatisticsFixture(8, quantizedCsv) { }
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
        auto score = model.getScore(model.getSettings()->getDivisionLevel());
        REQUIRE_THAT(score.total, WithinAbs(1, .001f));
    }
}

struct OffFixture : StatisticsFixture
{
    OffFixture() : StatisticsFixture(4, offCsv) {}
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
        auto score = model.getScore(model.getSettings()->getDivisionLevel());
        REQUIRE_THAT(score.total, WithinAbs(.76f, .001f));
    }
}

