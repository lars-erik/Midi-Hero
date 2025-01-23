#include "StatisticsPage.h"

StatisticsPage::StatisticsPage(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    audioProcessor(p),
    settings(p.settings),
    scoreCounts(p.model, p.settings),
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

    addAndMakeVisible(scoreCounts);

    addAndMakeVisible(divisionLevelSelector);

    p.model.observeNoteCount(&noteCountObserver, [&](int) { repaint(); });
}

StatisticsPage::~StatisticsPage()
{
    audioProcessor.model.stopObserveNoteCount(&noteCountObserver);
}


void StatisticsPage::paint(Graphics& g)
{
    g.fillAll(findColour(ResizableWindow::backgroundColourId));

    const auto score = audioProcessor.model.getScore(settings.getDivisionLevel());
    String scoreString(isnan(score.total) ? "N/A" : to_string(static_cast<int>(round(score.total * 100))));
    scoreLabel.setText(scoreString << "%", dontSendNotification);
    scoreNameLabel.setText(score.getScoreName(), dontSendNotification);
    scoreNameLabel.setColour(Label::textColourId, score.getColour());
}

void StatisticsPage::resized()
{
    scoreCounts.setBounds(25, 60, getWidth() / 2 - 37, getHeight() - 110);

    divisionLevelSelector.positionAtBottom();
}
