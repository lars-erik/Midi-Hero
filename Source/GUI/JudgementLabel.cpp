#include "JudgementLabel.h"

JudgementLabel::JudgementLabel()
{
    heroLabel.setFont(font);
    heroLabel.setAlpha(0);
    heroLabel.setInterceptsMouseClicks(false, false);
    setInterceptsMouseClicks(false, false);

    addAndMakeVisible(heroLabel);
}

void JudgementLabel::resized()
{
    auto width = TextLayout::getStringWidth(font, heroLabel.getText());
    heroLabel.centreWithSize(roundToInt(width), roundToInt(font.getHeight()));
    centerBounds = heroLabel.getBounds();
}

void JudgementLabel::setText(const std::string& text)
{
    heroLabel.setText(String(text), NotificationType::dontSendNotification);
    resized();
}

void JudgementLabel::setColour(Colour colour)
{
    heroLabel.setColour(Label::textColourId, colour);
}

void JudgementLabel::start()
{
    allOfIt.start();
    updater.addAnimator(allOfIt, [this]
        {
            updater.removeAnimator(allOfIt);
        });
}
