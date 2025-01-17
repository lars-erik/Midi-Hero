/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <sstream>
#include <iomanip>
#include <string>

std::string intToHex(int value) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << value;
    return oss.str();
}

//==============================================================================
MidiHeroAudioProcessorEditor::MidiHeroAudioProcessorEditor (MidiHeroAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), table(p.model)
{
    addAndMakeVisible(table);
    addAndMakeVisible(clearButton);
    addAndMakeVisible(copyButton);

    setResizable(true, true);
    centreWithSize(1024, 768);
    lastUIWidth.referTo(audioProcessor.state.getChildWithName("uiState").getPropertyAsValue("width", nullptr));
    lastUIHeight.referTo(audioProcessor.state.getChildWithName("uiState").getPropertyAsValue("height", nullptr));
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());

    lastUIWidth.addListener(this);
    lastUIHeight.addListener(this);

    clearButton.onClick = [&] { audioProcessor.model.clear(); };

    copyButton.onClick = [this]() { this->copyToClip(); };

    // From sample:
    // 
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (400, 300);

}

MidiHeroAudioProcessorEditor::~MidiHeroAudioProcessorEditor()
{
}

void MidiHeroAudioProcessorEditor::copyToClip()
{
    std::ostringstream str;
    str << "TimeInSeconds;PpqPosition;BarPpqPosition;Byte1;Byte2;Byte3;TimeStamp\r\n";
    for(const auto& msg : audioProcessor.model)
    {
        const uint8* rawData = msg.message.getRawData();
        const int rawSize = msg.message.getRawDataSize();
        if (rawData == nullptr || rawSize < 1) {
            throw std::runtime_error("Invalid MIDI message: raw data is null or too small.");
        }

        const uint8 byte1 = rawData[0]; // Always exists if rawSize >= 1
        const uint8 byte2 = (rawSize > 1) ? rawData[1] : 0; // Safely get byte2 if it exists
        const uint8 byte3 = (rawSize > 2) ? rawData[2] : 0; // Safely get byte3 if it exists

        str << msg.position.getTimeInSeconds().orFallback(0)
            << ";"
            << msg.position.getPpqPosition().orFallback(0)
            << ";"
            << msg.position.getPpqPositionOfLastBarStart().orFallback(0)
            << ";"
            << intToHex(byte1)
            << ";"
            << intToHex(byte2)
            << ";"
            << intToHex(byte3)
            << ";"
            << msg.message.getTimeStamp()
            << "\r\n";
    }

    const String juceString(str.str());
    SystemClipboard::copyTextToClipboard(juceString);
}

//==============================================================================
void MidiHeroAudioProcessorEditor::paint (juce::Graphics& g)
{
        // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void MidiHeroAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto buttonHeight = 24;
    auto buttonWidth = 50;
    auto buttonSpacing = 10; // Optional spacing between buttons

    // Reserve horizontal space for both buttons and center them
    auto buttonsWidth = (2 * buttonWidth) + buttonSpacing;
    auto buttonsArea = bounds.removeFromBottom(30).withSizeKeepingCentre(buttonsWidth, buttonHeight);

    // Set bounds for the first button (clearButton) and remove its area
    clearButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));

    // Add spacing if needed
    buttonsArea.removeFromLeft(buttonSpacing);

    // Set bounds for the second button (copyButton)
    copyButton.setBounds(buttonsArea.removeFromLeft(buttonWidth));

    table.setBounds(bounds);

    lastUIWidth = getWidth();
    lastUIHeight = getHeight();

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void MidiHeroAudioProcessorEditor::valueChanged(Value&)
{
    setSize(lastUIWidth.getValue(), lastUIHeight.getValue());
}