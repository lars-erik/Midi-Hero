/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
                       ),
    settings(state)
#endif
{
    // TODO: Use some other random generator?
    srand(static_cast<int>(time(nullptr)));  // NOLINT(cert-msc51-cpp, clang-diagnostic-shorten-64-to-32)

    state.addChild({ "uiState", { { "width",  600 }, { "height", 300 } }, {} }, -1, nullptr);
    startTimerHz(60);
}

MidiHeroAudioProcessor::~MidiHeroAudioProcessor() = default;

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

void MidiHeroAudioProcessor::setCurrentProgram (int)
{
}

const juce::String MidiHeroAudioProcessor::getProgramName (int)
{
    return "None";
}

void MidiHeroAudioProcessor::changeProgramName (int, const String&)
{
}

//==============================================================================
void MidiHeroAudioProcessor::prepareToPlay (double, int)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG("Prepare to play");
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

void MidiHeroAudioProcessor::process(const MidiBuffer& midi)
{
    Optional<AudioPlayHead::PositionInfo> posInfo;
    const double sampleRate = getSampleRate();
    if (const AudioPlayHead* currentPlayHead = getPlayHead())
    {
        posInfo = currentPlayHead->getPosition();
        if (posInfo)
        {
            const AudioPlayHead::PositionInfo value = *posInfo;
            timeAtProcess = *value.getTimeInSeconds();

            if (value.getIsPlaying() != isPlaying)
            {
                isPlaying = value.getIsPlaying();
                queue.push(MidiMessage(isPlaying ? 0xfa : 0xfc, 0, 0, 0), value, sampleRate);
            }
        }
    }

    if (posInfo)
    {
        queue.push(midi, *posInfo, sampleRate);
    }
    else
    {
        queue.push(midi, AudioPlayHead::PositionInfo(), sampleRate);
    }

}

void MidiHeroAudioProcessor::timerCallback() 
{
    std::vector<TimedMidiMessage> messages;
    queue.pop(std::back_inserter(messages));
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
        settings.reinitialize(state);
    }

    DBG(String("Deserialized\n\n") << state.toXmlString());
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiHeroAudioProcessor();
}

