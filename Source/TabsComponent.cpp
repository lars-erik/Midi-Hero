#include "TabsComponent.h"

#include "HeroPage.h"
#include "LogTableComponent.h"
#include "StatisticsComponent.h"

TabsComponent::TabsComponent(
    MidiHeroAudioProcessor& processor,
    TabbedButtonBar::Orientation orientation
)
    : TabbedComponent(orientation)
{
    auto colour = findColour(ResizableWindow::backgroundColourId);

    addTab("Hero", colour, new HeroPage(processor), true);
    addTab("Statistics", colour, new StatisticsComponent(processor), true);
    addTab("Log", colour, new LogTableComponent(processor), true);

    processor.model.observeIsPlaying(&isPlayingObserver, [&](const bool& v) {
        DBG("Is playing: " << to_string(v));

        this->setCurrentTabIndex(v ? 0 : 1, true);
    });
}
