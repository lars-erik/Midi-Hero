#pragma once

#include "Global.h"
#include "Observer.h"

class MidiHeroSettings
#ifdef UNIT_TESTS
    : CtorLogger
#endif
{
public:
    /*
     * TIMING SETTINGS
     */

    class TimingSettings
#ifdef UNIT_TESTS
        : CtorLogger
#endif
    {
    public:
        explicit TimingSettings(ValueTree& state)
            : state(state)
        {
            cout << "TimingSettings ctor " << this << endl;

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
        cout << "MidiHeroSettings ctor " << this << endl;

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

private:
    void initialize()
    {
        if (!state.hasType("settingsState"))
        {
            state.addChild({ "settingsState", {
                { "divisionLevel", 4 }
            }, {} }, -1, nullptr);
        }

        settingsState = state.getChildWithName("settingsState");

        DivisionLevel.referTo(settingsState.getPropertyAsValue("divisionLevel", nullptr));
    }

    ValueTree& state;
    ValueTree settingsState;
    TimingSettings timing;
};

