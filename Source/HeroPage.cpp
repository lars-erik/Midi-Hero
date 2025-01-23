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

    // TODO: Make division level selection component
    int selectedDivisionLevel = audioProcessor.settings.getDivisionLevel();

    const std::string buttonLabels[] = {
        "1/4",
        "1/8",
        "1/16",
        "1/32",
        "1/64"
    };

    const int buttonValues[] =
    {
        1,
        2,
        4,
        8,
        16
    };

    for(int i = 0; i < std::size(buttonLabels); i++)
    {
        auto* btn = new TextButton(buttonLabels[i]);
        btn->setClickingTogglesState(true);
        btn->setRadioGroupId(29999);
        btn->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != 4) ? Button::ConnectedOnRight : 0));

        if (buttonValues[i] == selectedDivisionLevel)
        {
            btn->setToggleState(true, dontSendNotification);
        }

        btn->onStateChange = [this, btn, buttonValues, i]()
        {
            const bool newState = btn->getToggleState();
            if (newState)
            {
                const int newValue = buttonValues[i];
                audioProcessor.settings.setDivisionLevel(newValue);
            }
        };

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

void HeroPage::valueChanged(Value&)
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



