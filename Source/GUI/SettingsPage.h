#pragma once
#include "Global.h"
#include "MidiHeroSettings.h"

class SettingsPage :
    public Component
{
public:
    SettingsPage(shared_ptr<MidiHeroSettings> const&);

    void resized() override;

private:
    OwnedArray<Component> children;

    shared_ptr<MidiHeroSettings> settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsPage)
};

