/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

// This is the ctor used by tests
MidiHeroAudioProcessor::MidiHeroAudioProcessor(bool startTimer, int queueSize)
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(getBusesLayout()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
#endif
    settings(make_shared<MidiHeroSettings>(state)),
    model(settings),
    queue(queueSize, settings)
{
    // TODO: Use some other random generator?
    srand(static_cast<int>(time(nullptr)));  // NOLINT(cert-msc51-cpp, clang-diagnostic-shorten-64-to-32)

    state.addChild({ "uiState", { { "width",  600 }, { "height", 300 } }, {} }, -1, nullptr);

    isPlaying = false;

    if (startTimer)
    {
        startTimerHz(60);
    }
}

// This is the ctor used by DAWs.
MidiHeroAudioProcessor::MidiHeroAudioProcessor()
    : MidiHeroAudioProcessor(true, 1 << 14)
{
}

MidiHeroAudioProcessor::~MidiHeroAudioProcessor() = default;

void MidiHeroAudioProcessor::process(const MidiBuffer& midi)
{
    Optional<AudioPlayHead::PositionInfo> posInfo;
    shared_ptr<AudioPlayHead::PositionInfo> posInfoPtr;
    const double sampleRate = getSampleRate();
    if (const AudioPlayHead* currentPlayHead = getPlayHead())
    {
        posInfo = currentPlayHead->getPosition();
        if (posInfo)
        {
            const AudioPlayHead::PositionInfo value = *posInfo;
            posInfoPtr = make_shared<AudioPlayHead::PositionInfo>(value);

            if (value.getIsPlaying() != isPlaying)
            {
                isPlaying = value.getIsPlaying();
                auto msg = MidiMessage(isPlaying ? 0xfa : 0xfc, 0, 0, 0);
                queue.push(msg, posInfoPtr, settings, sampleRate);
            }
        }
    }

    // TODO: Some day make sure we don't mix up posInfo and posInfoPtr's validity in memory. ><
    if (posInfo && isPlaying)
    {
        queue.push(midi, posInfoPtr, settings, sampleRate);
    }
}

bool MidiHeroAudioProcessor::hasQueuedItems() const
{
    return queue.size() > 0;
}


void MidiHeroAudioProcessor::timerCallback() 
{
    model.addMessages(queue);
}

//==============================================================================
bool MidiHeroAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiHeroAudioProcessor::createEditor()
{
    return new MidiHeroAudioProcessorEditor (*this);
}

//==============================================================================
void MidiHeroAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xmlState = state.createXml())
        copyXmlToBinary(*xmlState, destData);

    DBG("Serialized\n\n" << state.toXmlString());
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiHeroAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
    {
        state = ValueTree::fromXml(*xmlState);
        settings->reinitialize(state);
    }

    DBG(String("Deserialized\n\n") << state.toXmlString());
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}
