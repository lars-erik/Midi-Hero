#pragma once
#include "DivisionLevelComponent.h"
#include "Global.h"
#include "JudgementLabel.h"
#include "PluginProcessor.h"

class HeroPage :
    public Component
{
public:
    HeroPage(MidiHeroAudioProcessor&);
    ~HeroPage() override;

    void resized() override;

    void mouseUp(const MouseEvent&) override;

private:
    void scoreNewNotes();

    const int MaxLabels = 40;

    MidiHeroAudioProcessor& audioProcessor;

    Observer<int> noteCountObserver;

    OwnedArray<JudgementLabel> labels;
    int currentLabel = 0;

    DivisionLevelComponent divisionLevelSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroPage)
};

