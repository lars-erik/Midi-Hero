/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MidiHeroAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      private Value::Listener
{
public:
    MidiHeroAudioProcessorEditor (MidiHeroAudioProcessor&);
    ~MidiHeroAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void valueChanged(Value&) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiHeroAudioProcessor& audioProcessor;

    MidiTable table;
    TextButton clearButton{ "Clear" };

    Value lastUIWidth, lastUIHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiHeroAudioProcessorEditor)
};
