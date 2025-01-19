#include "TabsComponent.h"

#include "LogTableComponent.h"
#include "StatisticsComponent.h"

TabsComponent::TabsComponent(
    MidiHeroAudioProcessor& processor,
    TabbedButtonBar::Orientation orientation
)
    : TabbedComponent(orientation)
{
    auto colour = findColour(ResizableWindow::backgroundColourId);

    addTab("Statistics", colour, new StatisticsComponent(processor), true);
    addTab("Log", colour, new LogTableComponent(processor), true);

}
