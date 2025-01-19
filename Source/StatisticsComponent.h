#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

class StatisticsComponent :
    public Component
{
public:
    StatisticsComponent(MidiHeroAudioProcessor&);

    //void paint(juce::Graphics&) override;
    void resized() override;

private:
    MidiHeroAudioProcessor& audioProcessor;

    Label label { "hello", "Hello world!" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsComponent)
};

