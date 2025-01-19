#include "StatisticsComponent.h"

StatisticsComponent::StatisticsComponent(MidiHeroAudioProcessor& p) :
    Component("Statistics"),
    audioProcessor(p)
{
    addAndMakeVisible(label);
}

/*
void StatisticsComponent::paint(Graphics& g)
{
    
}
*/
void StatisticsComponent::resized()
{
    label.setBounds(getLocalBounds().reduced(10, 10));
}
