#include "HeroPage.h"

HeroPage::HeroPage(MidiHeroAudioProcessor& processor) :
    Component("Hero"),
    audioProcessor(processor)
{
    for(int i = 0; i<MaxLabels; i++)
    {
        auto label = new JudgementLabel();
        addAndMakeVisible(label);
        labels.add(label);
    }

    processor.model.addListener(this);
}

void HeroPage::resized()
{
    for (int i = 0; i < MaxLabels; i++)
    {
        labels[i]->centreWithSize(getWidth(), getHeight());
    }
}

void HeroPage::valueChanged(Value& value)
{
    // TODO: Configurable div. level
    const int divisionLevel = 4;

    auto newNotes = audioProcessor.model.getNewNotes();
    if (newNotes.empty()) return;

    auto score = MidiListModel::score(newNotes, divisionLevel);
    auto scoreName = score.getScoreName();

    DBG(newNotes.size() << " new notes, scored " << score.total << " with name " << scoreName);

    const int nextLabelIndex = currentLabel++ % MaxLabels;
    JudgementLabel* label = labels[nextLabelIndex];
    label->setText(scoreName);
    label->start();
}



