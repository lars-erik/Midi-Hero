#pragma once

#include "Global.h"
#include "PluginProcessor.h"

class TabsComponent :
    public TabbedComponent
{
public:
    TabsComponent(MidiHeroAudioProcessor& , TabbedButtonBar::Orientation);
    ~TabsComponent() override;

private:
    Observer<bool> isPlayingObserver;
    MidiHeroAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabsComponent)
};

