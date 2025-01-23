#include "TabsComponent.h"

#include "HeroPage.h"
#include "LogTablePage.h"
#include "StatisticsPage.h"

TabsComponent::TabsComponent(
    MidiHeroAudioProcessor& processor,
    TabbedButtonBar::Orientation orientation
)
    : TabbedComponent(orientation),
    processor(processor)
{
    auto colour = findColour(ResizableWindow::backgroundColourId);

    addTab("Hero", colour, new HeroPage(processor), true);
    addTab("Statistics", colour, new StatisticsPage(processor), true);
    addTab("Log", colour, new LogTablePage(processor), true);

    processor.model.observeIsPlaying(&isPlayingObserver, [&](const bool& v) {
        DBG("Is playing: " << to_string(v));

        this->setCurrentTabIndex(v ? 0 : 1, true);
    });
}

TabsComponent::~TabsComponent()
{
    processor.model.stopObserveIsPlaying(&isPlayingObserver);
}