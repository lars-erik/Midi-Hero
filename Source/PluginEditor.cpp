/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
MidiHeroAudioProcessorEditor::MidiHeroAudioProcessorEditor (MidiHeroAudioProcessor& p)
    : AudioProcessorEditor (&p),
    audioProcessor (p),
    tabs(p, TabbedButtonBar::TabsAtTop)
    //table(p.model)
{
    setResizable(true, true);
    centreWithSize(1024, 768);

    lastUIWidth.referTo(audioProcessor.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    lastUIHeight.referTo(audioProcessor.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);

    addAndMakeVisible(tabs);

}

MidiHeroAudioProcessorEditor::~MidiHeroAudioProcessorEditor() = default;

//==============================================================================
void MidiHeroAudioProcessorEditor::paint (juce::Graphics& g)
{
        // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MidiHeroAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    tabs.setBounds(bounds);

    lastUIWidth = getWidth();
    lastUIHeight = getHeight();

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void MidiHeroAudioProcessorEditor::valueChanged(Value&)
{
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
}