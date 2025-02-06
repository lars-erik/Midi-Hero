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
    Observer<bool> isPlayingObserver;

    const string HAVE_NOTES_TEXT = "Well played!\nGo to the statistics page to view your statistics.\nOr start the playhead again to get a new score.";
    const string NO_NOTES_TEXT = "Start the playhead and play some notes to get your first score!";

    Label pausedLabel = { "paused", NO_NOTES_TEXT };

    OwnedArray<JudgementLabel> labels;
    int currentLabel = 0;

    DivisionLevelComponent divisionLevelSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroPage)
};

