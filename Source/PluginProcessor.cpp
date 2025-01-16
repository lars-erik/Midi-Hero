/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

//==============================================================================
MidiHeroAudioProcessor::MidiHeroAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (getBusesLayout()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    state.addChild({ "uiState", { { "width",  600 }, { "height", 300 } }, {} }, -1, nullptr);
    startTimerHz(60);
}

MidiHeroAudioProcessor::~MidiHeroAudioProcessor()
{
}

//==============================================================================
const juce::String MidiHeroAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiHeroAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiHeroAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiHeroAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiHeroAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiHeroAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MidiHeroAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiHeroAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MidiHeroAudioProcessor::getProgramName (int index)
{
    return "None";
}

void MidiHeroAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MidiHeroAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void MidiHeroAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiHeroAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

template <typename Element>
void MidiHeroAudioProcessor::process(AudioBuffer<Element>& audio, MidiBuffer& midi)
{
    //audio.clear();
    queue.push(midi);

    if (const AudioPlayHead* currentPlayHead = getPlayHead())
    {
        if (Optional<AudioPlayHead::PositionInfo> posInfo = currentPlayHead->getPosition())
        {
            const AudioPlayHead::PositionInfo value = *posInfo;
            timeAtProcess = *value.getTimeInSeconds();
            if (value.getIsPlaying() != isPlaying)
            {
                isPlaying = value.getIsPlaying();
                queue.push(MidiMessage(isPlaying ? 0xfa : 0xfc, 0, 0, timeAtProcess));
            }
        }
    }

    // Don't think we need this?
    /*
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    */
}

void MidiHeroAudioProcessor::timerCallback() 
{
    std::vector<MidiMessage> messages;
    queue.pop(std::back_inserter(messages));
    for(MidiMessage& msg : messages)
    {
        if (!msg.isMidiStart() && !msg.isMidiStop())
        {
            msg.setTimeStamp(timeAtProcess);
        }
    }
    model.addMessages(messages.begin(), messages.end());
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

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiHeroAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary(data, sizeInBytes))
        state = ValueTree::fromXml(*xmlState);

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiHeroAudioProcessor();
}

