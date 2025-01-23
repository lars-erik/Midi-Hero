#pragma once
#include "Global.h"
#include "PluginProcessor.h"
#include "DivisionLevelComponent.h"

class StatisticsPage :
    public Component,
    private Value::Listener
{
public:
    StatisticsPage(MidiHeroAudioProcessor&);
    ~StatisticsPage() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void valueChanged(Value&) override;

    MidiHeroAudioProcessor& audioProcessor;
    MidiHeroSettings& settings;

    DivisionLevelComponent divisionLevelSelector;

    Label label { "statistics", "Statistics" };
    Label scoreLabel { "score", "N/A" };
    Label scoreNameLabel { "score", "N/A" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsPage)
};

