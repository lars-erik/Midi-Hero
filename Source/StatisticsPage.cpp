#include "StatisticsPage.h"

StatisticsPage::StatisticsPage(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    scoreCounts(p.model, p.settings),
    totalScore(p.model, p.settings),
    divisionLevelSelector(p.settings)
{
    auto font = Font(FontOptions(24, Font::FontStyleFlags::bold));

    int headerWidth = getWidth() / 2;
    statsHeader.setBounds(0, 20, headerWidth, 40);
    statsHeader.setFont(font);
    statsHeader.setJustificationType(Justification::Flags::centred);
    addAndMakeVisible(statsHeader);

    scoreHeader.setBounds(getWidth() / 2, 20, headerWidth, 40);
    scoreHeader.setFont(font);
    scoreHeader.setJustificationType(Justification::Flags::centred);
    addAndMakeVisible(scoreHeader);

    addAndMakeVisible(scoreCounts);
    addAndMakeVisible(totalScore);

    addAndMakeVisible(divisionLevelSelector);

}

void StatisticsPage::resized()
{
    int headerWidth = getWidth() / 2;
    statsHeader.setBounds(0, 20, headerWidth, 40);
    scoreHeader.setBounds(getWidth() / 2, 20, headerWidth, 40);

    scoreCounts.setBounds(25, 70, getWidth() / 2 - 37, getHeight() - 120);
    totalScore.setBounds(getWidth() / 2 + 12, 70, getWidth() / 2 - 37, getHeight() - 120);

    divisionLevelSelector.positionAtBottom();
}
