#pragma once
#include "DivisionLevelComponent.h"
#include "Global.h"
#include "JudgementLabel.h"
#include "PluginProcessor.h"

class HeroPage :
    public Component,
    private Value::Listener
{
public:
    HeroPage(MidiHeroAudioProcessor&);
    ~HeroPage() override = default;

    void resized() override;

    void valueChanged(Value& value) override;

    void mouseUp(const MouseEvent&) override;

private:
    const int MaxLabels = 40;

    MidiHeroAudioProcessor& audioProcessor;

    OwnedArray<JudgementLabel> labels;
    int currentLabel = 0;

    DivisionLevelComponent divisionLevelSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroPage)
};

