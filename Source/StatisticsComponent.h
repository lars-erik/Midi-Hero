#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

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

    Label label { "statistics", "Statistics" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsComponent)
};

