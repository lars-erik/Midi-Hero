#pragma once
#include "Global.h"
#include "PluginProcessor.h"
#include "DivisionLevelComponent.h"
#include "ScoreCountsComponent.h"

class StatisticsPage :
    public Component
{
public:
    StatisticsPage(MidiHeroAudioProcessor&);
    ~StatisticsPage() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    MidiHeroAudioProcessor& audioProcessor;
    MidiHeroSettings& settings;

    ScoreCountsComponent scoreCounts;
    DivisionLevelComponent divisionLevelSelector;

    Observer<int> noteCountObserver;

    Label label { "statistics", "Statistics" };
    Label scoreLabel { "score", "N/A" };
    Label scoreNameLabel { "score", "N/A" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsPage)
};

