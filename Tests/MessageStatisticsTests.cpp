#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>
#include "catch2/catch_all.hpp"

#include <numeric>

#include "TestData.h"
#include "TestUtils.h"

TEST_CASE("Note calculations for quantized play")
{
    constexpr int DivisionLevel = 8;
    auto model = getTestData(quantizedCsv);
    auto notes = model.getNotes();

    string report = buildReport(notes, DivisionLevel);

    ApprovalTests::Approvals::verify(report);
}

TEST_CASE("Quantized score is 100%")
{
    constexpr int DivisionLevel = 8;
    auto model = getTestData(quantizedCsv);
    auto notes = model.getNotes();

    vector<double> scores;
    transform(notes.begin(), notes.end(), back_inserter(scores), [](const shared_ptr<TimedMidiMessage> m) { return m->getScore(DivisionLevel); });
    double score = accumulate(scores.begin(), scores.end(), 0.0);
    double totalScore = score / notes.size();

    REQUIRE(totalScore == 1);
}

TEST_CASE("Note calculations for off play")
{
    constexpr int DivisionLevel = 4;

    auto model = getTestData(offCsv);
    auto notes = model.getNotes();

    string report = buildReport(notes, DivisionLevel);

    ApprovalTests::Approvals::verify(report);
}

TEST_CASE("Off score is 76%")
{
    constexpr int DivisionLevel = 4;
    auto model = getTestData(offCsv);

    auto score = model.getScore(DivisionLevel);

    INFO("Testing appx " << score.total << " ~ " << .76);
    REQUIRE(approx(score.total, .76));
}