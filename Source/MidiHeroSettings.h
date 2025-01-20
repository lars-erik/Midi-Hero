#pragma once

#include "JuceHeader.h"
using namespace juce;

class MidiHeroSettings
{
public:
    explicit MidiHeroSettings(ValueTree& state) :
        state(state)
    {
        initialize();
    }

    void reinitialize(const ValueTree& newState)
    {
        state = newState;

        initialize();
    }

    int getDivisionLevel() const
    {
        return divisionLevel.getValue();
    }

    void setDivisionLevel(const int value)
    {
        divisionLevel.setValue(value);
    }

private:
    void initialize()
    {
        if (!state.hasType("settingsState"))
        {
            state.addChild({ "settingsState", { { "divisionLevel", 4 } }, {} }, -1, nullptr);
        }

        settingsState = state.getChildWithName("settingsState");
        divisionLevel.referTo(settingsState.getPropertyAsValue("divisionLevel", nullptr));
    }

    ValueTree& state;
    ValueTree settingsState;

    Value divisionLevel;
};

