#pragma once

#include "Global.h"
#include "PluginProcessor.h"

class DivisionLevelComponent :
    public Component
{
public:
    DivisionLevelComponent(const shared_ptr<MidiHeroSettings>& s) :
        settings(s)
    {
        int selectedDivisionLevel = settings->getDivisionLevel();

        for (int i = 0; i < std::size(buttonLabels); i++)
        {
            auto* btn = new TextButton(buttonLabels[i]);
            btn->setClickingTogglesState(true);
            btn->setRadioGroupId(29999);
            btn->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != 4) ? Button::ConnectedOnRight : 0));

            if (buttonValues[i] == selectedDivisionLevel)
            {
                btn->setToggleState(true, dontSendNotification);
            }

            btn->onStateChange = [this, btn, i]()
            {
                if (btn->getToggleState())
                {
                    settings->setDivisionLevel(buttonValues[i]);
                }
            };

            buttons.add(btn);
            addAndMakeVisible(btn);
        }

        settings->observeDivisionLevel(&settingObserver, [&](int newDivisionLevel)
        {
            for(int i = 0; i<5; i++)
            {
                if (buttonValues[i] == newDivisionLevel)
                {
                    buttons[i]->setToggleState(true, dontSendNotification);
                    break;
                }
            }
        });
    }

    void positionAtBottom()
    {
        setBounds(getParentWidth() / 2 - 125, getParentHeight() - 50, 250, 30);
    }

    void resized() override
    {
        for (int i = 0; i < 5; i++)
        {
            buttons[i]->setBounds(i * 50, 0, 50, 30);
        }
    }

private:
    const shared_ptr<MidiHeroSettings> settings;
    Observer<int> settingObserver;

    OwnedArray<TextButton> buttons;

    const std::string buttonLabels[5] = {
        "1/4",
        "1/8",
        "1/16",
        "1/32",
        "1/64"
    };

    const int buttonValues[5] =
    {
        1,
        2,
        4,
        8,
        16
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DivisionLevelComponent)
};
