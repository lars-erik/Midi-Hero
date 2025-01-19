#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

class TabsComponent :
    public TabbedComponent
{
public:
    TabsComponent(MidiHeroAudioProcessor& , TabbedButtonBar::Orientation);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabsComponent)
};

