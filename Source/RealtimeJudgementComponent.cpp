#include "RealtimeJudgementComponent.h"

RealtimeJudgementComponent::RealtimeJudgementComponent()
{
    heroLabel.setFont(font);
    heroLabel.setAlpha(0);
    heroLabel.setInterceptsMouseClicks(false, false);

    addAndMakeVisible(heroLabel);
}

void RealtimeJudgementComponent::resized()
{
    auto width = TextLayout::getStringWidth(font, heroLabel.getText());
    heroLabel.centreWithSize(roundToInt(width), roundToInt(font.getHeight()));
    centerBounds = heroLabel.getBounds();
}

void RealtimeJudgementComponent::mouseUp(const MouseEvent& mouse_event)
{
    allOfIt.start();
    updater.addAnimator(allOfIt, [this]
        {
            updater.removeAnimator(allOfIt);
        });
}
