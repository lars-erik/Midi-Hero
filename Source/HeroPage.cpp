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

    const std::string buttonLabels[] = {
        "1/4",
        "1/8",
        "1/16",
        "1/32",
        "1/64"
    };

    for(int i = 0; i < std::size(buttonLabels); i++)
    {
        auto* btn = new TextButton(buttonLabels[i]);
        btn->setClickingTogglesState(true);
        btn->setRadioGroupId(29999);
        btn->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != 4) ? Button::ConnectedOnRight : 0));

        children.add(btn);
        addAndMakeVisible(btn);
    }

    processor.model.addListener(this);
}

void HeroPage::resized()
{
    for (int i = 0; i < MaxLabels; i++)
    {
        labels[i]->centreWithSize(getWidth(), getHeight());
    }

    // TODO: Remove magic numbers
    auto bounds = getLocalBounds();
    auto br = bounds.getBottomRight();
    Point startPoint = br.transformedBy(AffineTransform::translation(static_cast<float>(br.getX()) / -2.0f - 125.5f, -50));
    for (int i = 0; i<5; i++)
    {
        children[i]->setBounds(startPoint.getX() + i * 50, startPoint.getY(), 50, 30);
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



