#pragma once
#include "Global.h"
#include "PluginProcessor.h"
#include "DivisionLevelComponent.h"
#include "ScoreCountsComponent.h"
#include "TotalScoreComponent.h"

class StatisticsPage :
    public Component
{
public:
    StatisticsPage(MidiHeroAudioProcessor&);
    ~StatisticsPage() override = default;

    void resized() override;

private:
    ScoreCountsComponent scoreCounts;
    TotalScoreComponent totalScore;

    DivisionLevelComponent divisionLevelSelector;

    Label statsHeader { "statistics", "Statistics" };
    Label scoreHeader { "score", "Score" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsPage)
};

