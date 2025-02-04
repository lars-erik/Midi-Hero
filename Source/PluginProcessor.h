/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "Global.h"

#include "MidiHeroSettings.h"
#include "MidiListModel.h"
#include "MidiQueue.h"

//==============================================================================
/**
*/
class MidiHeroAudioProcessor  : public juce::AudioProcessor,
                                private juce::Timer
{
public:
    //==============================================================================
    MidiHeroAudioProcessor();
    // Here for tests
    MidiHeroAudioProcessor(bool startTimer, int queueSize);
    ~MidiHeroAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void process(const MidiBuffer& midi);

    void processBlock(AudioBuffer<float>&, MidiBuffer& midi) override { process(midi); }
    void processBlock(AudioBuffer<double>&, MidiBuffer& midi) override { process(midi); }
    
    void timerCallback() override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool hasQueuedItems() const;

    ValueTree state{ "state" };
    shared_ptr<MidiHeroSettings> settings;
    MidiListModel model;

private:
    MidiQueue queue;
    bool isPlaying;
    double timeAtProcess;

    static BusesProperties getBusesLayout()
    {
        // Live and Cakewalk don't like to load midi-only plugins, so we add an audio output there.
        const PluginHostType host;
        return host.isAbletonLive() || host.isSonar()
            ? BusesProperties()
                .withInput("Input", AudioChannelSet::disabled(), false)
                .withOutput("Output", AudioChannelSet::stereo(), false)
            : BusesProperties();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiHeroAudioProcessor)
};
