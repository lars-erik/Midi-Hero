#pragma once
#include "Global.h"
#include "PluginProcessor.h"

class StatisticsComponent :
    public Component,
    private Value::Listener
{
public:
    StatisticsComponent(MidiHeroAudioProcessor&);
    ~StatisticsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void valueChanged(Value&) override;

    MidiHeroAudioProcessor& audioProcessor;
    MidiHeroSettings& settings;

    Label label { "statistics", "Statistics" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsComponent)
};

