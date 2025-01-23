#pragma once

#include "Global.h"
#include "MidiHeroSettings.h"
#include "MidiListModel.h"

class ScoreCountsComponent :
    public Component
{
public:
    ScoreCountsComponent(
        MidiListModel& model,
        MidiHeroSettings& settings
    ): model(model), settings(settings)
    {
        
    }

    void paint(Graphics& g) override
    {
        g.fillAll(findColour(ResizableWindow::backgroundColourId));

        int divisionLevel = settings.getDivisionLevel();
        const auto score = model.getScore(divisionLevel);
        auto scores = model.getScoreCounts(divisionLevel);

        // TODO: Max?
        const int maxBarWidth = max(getLocalBounds().getWidth() - 100, 100);

        int y = 0;
        for (const std::string& key : MidiListModel::Scoring::keys)
        {
            g.setColour(findColour(Label::textColourId));
            g.drawText(String(key + ": "), 0, y, 80, 25, Justification::centredLeft);

            const int width = static_cast<int>(static_cast<double>(scores[key]) / score.notes * maxBarWidth);
            g.setColour(MidiListModel::Scoring::getColour(key));
            g.fillRect(100, y, width, 25);

            y += 30;
        }
    }

private:
    MidiListModel& model;
    MidiHeroSettings& settings;
};
