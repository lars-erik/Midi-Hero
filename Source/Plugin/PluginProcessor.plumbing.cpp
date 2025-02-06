#include "PluginProcessor.h"

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

void MidiHeroAudioProcessor::setCurrentProgram(int)
{
}

const juce::String MidiHeroAudioProcessor::getProgramName(int)
{
    return "None";
}

void MidiHeroAudioProcessor::changeProgramName(int, const String&)
{
}

//==============================================================================
void MidiHeroAudioProcessor::prepareToPlay(double, int)
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
bool MidiHeroAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiHeroAudioProcessor();
}

