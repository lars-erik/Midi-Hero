#include "HeroPage.h"

HeroPage::HeroPage(MidiHeroAudioProcessor& processor) :
    Component("Hero"),
    audioProcessor(processor),
    divisionLevelSelector(processor.settings)
{
    for(int i = 0; i<MaxLabels; i++)
    {
        auto label = new JudgementLabel();
        addAndMakeVisible(label);
        labels.add(label);
    }

    addAndMakeVisible(divisionLevelSelector);

    processor.model.observeNoteCount(&noteCountObserver, [&](int) { scoreNewNotes(); });
}

HeroPage::~HeroPage()
{
    audioProcessor.model.stopObserveNoteCount(&noteCountObserver);
}

void HeroPage::resized()
{
    for (int i = 0; i < MaxLabels; i++)
    {
        labels[i]->centreWithSize(getWidth(), getHeight());
    }

    divisionLevelSelector.positionAtBottom();
}

void HeroPage::scoreNewNotes()
{
    auto newNotes = audioProcessor.model.getNewNotes();
    if (newNotes.empty()) return;

    auto score = MidiListModel::getScore(newNotes, audioProcessor.settings.getDivisionLevel());
    auto scoreName = score.getScoreName();
    const Colour colour = MidiListModel::Scoring::getColour(scoreName);

    DBG(newNotes.size() << " new notes, scored " << score.total << " with name " << scoreName);

    const int nextLabelIndex = currentLabel++ % MaxLabels;
    JudgementLabel* label = labels[nextLabelIndex];
    label->setText(scoreName);
    label->setColour(colour);
    label->start();
}

void HeroPage::mouseUp(const MouseEvent&)
{
    const double value = std::rand();  // NOLINT(concurrency-mt-unsafe)
    const double ratio = value / RAND_MAX;
    const int it = static_cast<int>(floor(ratio * MidiListModel::Scoring::keys.size()));
    const std::string scoreName = MidiListModel::Scoring::keys[it];
    const Colour colour = MidiListModel::Scoring::getColour(scoreName);
    const int nextLabelIndex = currentLabel++ % MaxLabels;
    JudgementLabel* label = labels[nextLabelIndex];
    label->setText(scoreName);
    label->setColour(colour);
    label->start();
}



