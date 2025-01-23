#pragma once

#include "Global.h"
#include "Observer.h"

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

    OBSERVABLE(int, DivisionLevel)

private:
    void initialize()
    {
        if (!state.hasType("settingsState"))
        {
            state.addChild({ "settingsState", { { "divisionLevel", 4 } }, {} }, -1, nullptr);
        }

        settingsState = state.getChildWithName("settingsState");
        DivisionLevel.referTo(settingsState.getPropertyAsValue("divisionLevel", nullptr));
    }

    ValueTree& state;
    ValueTree settingsState;
};

