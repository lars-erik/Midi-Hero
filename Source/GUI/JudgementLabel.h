#pragma once
#include "Global.h"

class JudgementLabel :
    public Component
{
public:
    JudgementLabel();

    void resized() override;
    void setText(const std::string& text);
    void setColour(Colour colour);
    void start();

private:
    Label heroLabel { "hero", "Perfect" };
    Font font{ FontOptions(96, Font::FontStyleFlags::bold) };

    Rectangle<int> centerBounds;

    Animator waitABit = ValueAnimatorBuilder()
        .withDurationMs(50.f)
        .build();

    Animator slideInAnimator = ValueAnimatorBuilder()
        .withDurationMs(50.f)
        .withEasing(Easings::createEaseOut())
        .withOnStartReturningValueChangedCallback(
            [this]
            {
                auto limits = makeAnimationLimits(50.f, 0.f);

                return [this, limits](auto value)
                {
                    const auto progress = std::clamp(value, 0.f, 1.f);
                    heroLabel.setTransform(AffineTransform::translation(0, limits.lerp(progress)));
                    heroLabel.setAlpha(progress);
                };
            }
        )
        .withOnCompleteCallback(
            [this]
            {
                heroLabel.setTransform(AffineTransform::translation(0, 0));
                heroLabel.setAlpha(1);
            }
        )
        .build();

    Animator slideOutAnimator = ValueAnimatorBuilder()
        .withDurationMs(400.f)
        .withEasing(Easings::createEaseIn())
        .withOnStartReturningValueChangedCallback(
            [this]
            {
                auto limits = makeAnimationLimits(0.f, -150.f);

                return [this, limits](auto value)
                {
                    const auto progress = std::clamp(value, 0.f, 1.f);
                    heroLabel.setTransform(AffineTransform::translation(0, limits.lerp(progress)));
                    heroLabel.setAlpha(1.f - progress);
                };
            }
        )
        .withOnCompleteCallback(
            [this]
            {
                heroLabel.setTransform(AffineTransform::translation(0, 0));
                heroLabel.setAlpha(0);
            }
        )
        .build();

    Animator allOfIt = AnimatorSetBuilder([] {})
        .followedBy(slideInAnimator)
        //.followedBy(waitABit)
        .followedBy(slideOutAnimator)
        .build();

    VBlankAnimatorUpdater updater{ this };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JudgementLabel)
};

