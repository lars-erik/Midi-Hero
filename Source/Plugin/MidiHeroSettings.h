#pragma once

#include "Global.h"
#include "Observer.h"

class MidiHeroSettings
{
public:
    /*
     * TIMING SETTINGS
     */

    class TimingSettings
    {
    public:
        explicit TimingSettings(ValueTree& state)
            : state(state)
        {
            initialize();
        }

        void reinitialize(const ValueTree& newState)
        {
            state = newState;
            initialize();
        }

        int getScaledPerfectMs() const { return scaled(getPerfectMs()); }
        int getScaledGreatMs() const { return scaled(getGreatMs()); }
        int getScaledGoodMs() const { return scaled(getGoodMs()); }
        int getScaledOffMs() const { return scaled(getOffMs()); }

        OBSERVABLE(int, PerfectMs);
        OBSERVABLE(int, GreatMs);
        OBSERVABLE(int, GoodMs);
        OBSERVABLE(int, OffMs);

        OBSERVABLE(float, Scale);

    private:
        int scaled(int value) const
        {
            return static_cast<int>(floor(getScale() * static_cast<float>(value)));
        }

        void initialize()
        {
            if (!state.hasType("timing"))
            {
                state.addChild({ "timing" , {
                    { "perfectMs", 10 },
                    { "greatMs",   20 },
                    { "goodMs",    40 },
                    { "offMs",     80 },
                    { "scale",      1.0 }
                }, {} }, -1, nullptr);
            }

            timingState = state.getChildWithName("timing");

            PerfectMs.referTo(timingState.getPropertyAsValue("perfectMs", nullptr));
            GreatMs.referTo(timingState.getPropertyAsValue("greatMs", nullptr));
            GoodMs.referTo(timingState.getPropertyAsValue("goodMs", nullptr));
            OffMs.referTo(timingState.getPropertyAsValue("offMs", nullptr));

            Scale.referTo(timingState.getPropertyAsValue("scale", nullptr));
        }

        ValueTree& state;
        ValueTree timingState;
    };

    /*
     * ROOT SETTINGS
     */

    explicit MidiHeroSettings(ValueTree& state) :
        state(state),
        timing(state)
    {
        initialize();
    }

    void reinitialize(const ValueTree& newState)
    {
        state = newState;
        timing.reinitialize(newState);

        initialize();
    }

    TimingSettings& getTiming() { return timing; }

    OBSERVABLE(int, DivisionLevel)
    OBSERVABLE(int, RangeToKeep)

private:
    void initialize()
    {
        if (!state.hasType("settingsState"))
        {
            state.addChild({ "settingsState", {
                { "divisionLevel", 4 },
                { "rangeToKeep", 20 }
            }, {} }, -1, nullptr);
        }

        settingsState = state.getChildWithName("settingsState");

        DivisionLevel.referTo(settingsState.getPropertyAsValue("divisionLevel", nullptr));
        RangeToKeep.referTo(settingsState.getPropertyAsValue("rangeToKeep", nullptr));
    }

    ValueTree& state;
    ValueTree settingsState;
    TimingSettings timing;
};

