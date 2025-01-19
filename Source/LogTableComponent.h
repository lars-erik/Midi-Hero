#pragma once

#include <JuceHeader.h>

#include "MidiUtils.h"
#include "PluginProcessor.h"

using namespace juce;

class LogTableComponent :
    public Component    
{
public:
    LogTableComponent(MidiHeroAudioProcessor&);
    ~LogTableComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void copyToClip() const;

    MidiHeroAudioProcessor& audioProcessor;
    MidiTable table;
    TextButton clearButton{ "Clear" };
    TextButton copyButton{ "Copy" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogTableComponent)
};