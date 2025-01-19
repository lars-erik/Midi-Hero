#include "StatisticsComponent.h"

StatisticsComponent::StatisticsComponent(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    audioProcessor(p)
{
    label.setBounds(20, 20, 100, 20);
    label.setJustificationType(Justification::Flags::centredLeft);
    addAndMakeVisible(label);

    p.model.addListener(this);
}

StatisticsComponent::~StatisticsComponent()
{
    audioProcessor.model.removeListener(this);
}

void StatisticsComponent::paint(Graphics& g)
{
    // TODO: Configurable division level
    const int divisionLevel = 4;

    g.fillAll(findColour(ResizableWindow::backgroundColourId));

    auto notes = audioProcessor.model.filterMessages([](const TimedMidiMessage& m) {return m.message.isNoteOn(); });
    int total = notes.size();

    std::vector<std::string> keys = {
        "Perfect", "Great", "Good", "Off", "Bad"
    };

    std::map<std::string, int> scores = {
        {"Perfect", 0 },
        {"Great", 0 },
        {"Good", 0 },
        {"Off", 0 },
        {"Bad", 0 }
    };

    std::unordered_map<std::string, Colour> colors = {
        {"Perfect", Colours::green },
        {"Great", Colours::lightgreen },
        {"Good", Colours::greenyellow },
        {"Off", Colours::orange },
        {"Bad", Colours::red }
    };

    for(TimedMidiMessage& m : notes)
    {
        scores[m.getScoreName(divisionLevel)]++;
    }

    const int maxBarWidth = 200;

    int y = 10;
    for(const std::string key : keys)
    {
        y += 40;

        g.setColour(findColour(Label::textColourId));
        g.drawText(String(key + ": "), 25, y, 80, 30, Justification::centredLeft);

        const int width = static_cast<int>(static_cast<double>(scores[key]) / total * maxBarWidth);
        g.setColour(colors[key]);
        g.fillRect(100, y, width, 30);
    }
}

void StatisticsComponent::resized()
{
}

void StatisticsComponent::valueChanged(Value& v)
{
    repaint();
}
