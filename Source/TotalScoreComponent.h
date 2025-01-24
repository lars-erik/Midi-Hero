#pragma once

#include "Global.h"
#include "MidiHeroSettings.h"
#include "MidiListModel.h"

class TotalScoreComponent :
    public Component
{
public:
    TotalScoreComponent(
        MidiListModel& model,
        MidiHeroSettings& settings
    ) :
        model(model),
        settings(settings)
    {
        scoreLabel.setBounds(0, 0, getWidth(), 100);
        scoreLabel.setJustificationType(Justification::Flags::centred);
        scoreLabel.setFont(Font(FontOptions(128, Font::FontStyleFlags::bold)));
        addAndMakeVisible(scoreLabel);

        scoreNameLabel.setBounds(0, 125, getWidth(), 100);
        scoreNameLabel.setJustificationType(Justification::Flags::centred);
        scoreNameLabel.setFont(Font(FontOptions(76, Font::FontStyleFlags::bold)));
        addAndMakeVisible(scoreNameLabel);

        model.observeNoteCount(&noteCountObserver, [&](int) { recalculate(); });
        settings.observeDivisionLevel(&divisionLevelObserver, [&](int) { recalculate(); });
    }

    ~TotalScoreComponent() override
    {
        model.stopObserveNoteCount(&noteCountObserver);
        settings.stopObserveDivisionLevel(&divisionLevelObserver);
    }

    void resized() override
    {
        scoreLabel.setBounds(0, 0, getWidth(), 100);
        scoreNameLabel.setBounds(0, 125, getWidth(), 100);
    }

private:
    void recalculate()
    {
        const auto score = model.getScore(settings.getDivisionLevel());
        String scoreString(isnan(score.total) ? "" : to_string(static_cast<int>(round(score.total * 100))) + "%");
        scoreLabel.setText(scoreString, dontSendNotification);
        scoreNameLabel.setText(score.notes == 0 ? "" : score.getScoreName(), dontSendNotification);
        scoreNameLabel.setColour(Label::textColourId, score.getColour());
    }

    Observer<int> noteCountObserver;
    Observer<int> divisionLevelObserver;

    MidiListModel& model;
    MidiHeroSettings settings;

    Label scoreLabel{ "score", "N/A" };
    Label scoreNameLabel{ "scoreName", "N/A" };
};
