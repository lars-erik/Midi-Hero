#pragma once

#include "Global.h"
#include "PluginProcessor.h"

class TabsComponent :
    public TabbedComponent
{
public:
    TabsComponent(MidiHeroAudioProcessor& , TabbedButtonBar::Orientation);

private:
    Observer<bool> isPlayingObserver;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabsComponent)
};

