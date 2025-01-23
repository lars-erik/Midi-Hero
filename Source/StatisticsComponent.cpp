#include "StatisticsComponent.h"

StatisticsComponent::StatisticsComponent(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    audioProcessor(p),
    settings(p.settings),
    divisionLevelSelector(p.settings)
{
    label.setBounds(20, 20, 100, 20);
    label.setJustificationType(Justification::Flags::centredLeft);
    addAndMakeVisible(label);

    scoreLabel.setBounds(300, 50, 400, 100);
    scoreLabel.setJustificationType(Justification::Flags::centred);
    scoreLabel.setFont(Font(FontOptions(128, Font::FontStyleFlags::bold)));
    addAndMakeVisible(scoreLabel);

    scoreNameLabel.setBounds(300, 175, 400, 100);
    scoreNameLabel.setJustificationType(Justification::Flags::centred);
    scoreNameLabel.setFont(Font(FontOptions(76, Font::FontStyleFlags::bold)));
    addAndMakeVisible(scoreNameLabel);

    addAndMakeVisible(divisionLevelSelector);

    p.model.addListener(this);
}

StatisticsComponent::~StatisticsComponent()
{
    audioProcessor.model.removeListener(this);
}

void StatisticsComponent::paint(Graphics& g)
{
    g.fillAll(findColour(ResizableWindow::backgroundColourId));

    const auto score = audioProcessor.model.getScore(settings.getDivisionLevel());
    String scoreString(isnan(score.total) ? "N/A" : to_string(static_cast<int>(round(score.total * 100))));
    scoreLabel.setText(scoreString << "%", dontSendNotification);
    scoreNameLabel.setText(score.getScoreName(), dontSendNotification);
    scoreNameLabel.setColour(Label::textColourId, score.getColour());

    // TODO: Combine in one struct.
    auto scores = audioProcessor.model.getScoreCounts(settings.getDivisionLevel());
    const auto total = score.notes;

    const int maxBarWidth = 250;

    int y = 10;
    for(const std::string key : MidiListModel::Scoring::keys)
    {
        y += 40;

        g.setColour(findColour(Label::textColourId));
        g.drawText(String(key + ": "), 25, y, 80, 30, Justification::centredLeft);

        const int width = static_cast<int>(static_cast<double>(scores[key]) / total * maxBarWidth);
        g.setColour(MidiListModel::Scoring::getColour(key));
        g.fillRect(100, y, width, 30);
    }


}

void StatisticsComponent::resized()
{
    divisionLevelSelector.positionAtBottom();
}

void StatisticsComponent::valueChanged(Value&)
{
    repaint();
}
