#pragma once

#include "Global.h"

#include "PluginProcessor.h"
#include "MidiTable.h"

class LogTablePage :
    public Component    
{
public:
    LogTablePage(MidiHeroAudioProcessor&);
    ~LogTablePage() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void copyToClip() const;

    MidiHeroAudioProcessor& audioProcessor;
    MidiTable table;

    TextButton clearButton{ "Clear" };
    TextButton copyButton{ "Copy" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogTablePage)
};