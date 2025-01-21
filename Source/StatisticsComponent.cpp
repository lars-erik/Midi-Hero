#include "StatisticsComponent.h"

StatisticsComponent::StatisticsComponent(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    audioProcessor(p),
    settings(p.settings)
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
    g.fillAll(findColour(ResizableWindow::backgroundColourId));

    // TODO: Combine in one struct.
    auto scores = audioProcessor.model.getScoreCounts(settings.getDivisionLevel());
    const auto total = audioProcessor.model.getNotes().size();

    const int maxBarWidth = 200;

    int y = 10;
    for(const std::string key : MidiListModel::Scoring::keys)
    {
        y += 40;

        g.setColour(findColour(Label::textColourId));
        g.drawText(String(key + ": "), 25, y, 80, 30, Justification::centredLeft);

        const int width = static_cast<int>(static_cast<double>(scores[key]) / total * maxBarWidth);
        g.setColour(MidiListModel::Scoring::colors[key]);
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
