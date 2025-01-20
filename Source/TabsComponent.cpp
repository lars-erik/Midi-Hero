#include "TabsComponent.h"

#include "LogTableComponent.h"
#include "StatisticsComponent.h"
#include "JudgementLabel.h"

TabsComponent::TabsComponent(
    MidiHeroAudioProcessor& processor,
    TabbedButtonBar::Orientation orientation
)
    : TabbedComponent(orientation)
{
    auto colour = findColour(ResizableWindow::backgroundColourId);

    addTab("Hero", colour, new JudgementLabel(), true);
    addTab("Statistics", colour, new StatisticsComponent(processor), true);
    addTab("Log", colour, new LogTableComponent(processor), true);
}
