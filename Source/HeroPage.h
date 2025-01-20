#pragma once
#include "JuceHeader.h"
#include "JudgementLabel.h"
#include "PluginProcessor.h"

using namespace juce;

class HeroPage :
    public Component,
    private Value::Listener
{
public:
    HeroPage(MidiHeroAudioProcessor&);
    ~HeroPage() override = default;

    void resized() override;

    void valueChanged(Value& value) override;

private:
    const int MaxLabels = 40;

    MidiHeroAudioProcessor& audioProcessor;

    OwnedArray<JudgementLabel> labels;
    int currentLabel = 0;

    OwnedArray<Component> children;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroPage)
};

