#include "LogTablePage.h"

std::string intToHex(int value) {
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << value;
    return oss.str();
}

LogTablePage::LogTablePage(MidiHeroAudioProcessor& audioProcessor) :
    Component("Log table"),
    audioProcessor(audioProcessor),
    table(audioProcessor.model, audioProcessor.settings)
{
    addAndMakeVisible(table);
    addAndMakeVisible(clearButton);
    addAndMakeVisible(copyButton);

    clearButton.onClick = [&] { audioProcessor.model.clear(); };
    copyButton.onClick = [&]() { copyToClip(); };
}

LogTablePage::~LogTablePage()
{
}

void LogTablePage::copyToClip() const
{
    std::ostringstream str;
    str << "TimeInSeconds;PpqPosition;BarPpqPosition;Byte1;Byte2;Byte3;TimeStamp\r\n";
    for (const auto& msg : audioProcessor.model)
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
void LogTablePage::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}

void LogTablePage::resized()
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

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
