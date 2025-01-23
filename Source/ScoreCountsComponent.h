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

        double maxPercent = 0;
        for (const std::string& key : MidiListModel::Scoring::keys)
        {
            maxPercent = max(static_cast<double>(scores[key]) / score.notes, maxPercent);
        }

        const int maxBarWidth = max(static_cast<int>(static_cast<double>(getLocalBounds().getWidth() - 185) / maxPercent), 50);

        int y = 0;
        int i = 0;
        string ms[] = { "<10ms", "<20ms", "<40ms", "<80ms", ">=80ms" };
        for (const std::string& key : MidiListModel::Scoring::keys)
        {
            g.setColour(findColour(Label::textColourId));
            g.drawText(String(key + " (" + ms[i++] + "): "), 0, y, 110, 25, Justification::centredLeft, false);

            double percent = static_cast<double>(scores[key]) / score.notes;
            const int width = static_cast<int>(percent * maxBarWidth);

            g.drawText(String(scores[key]) + " (" + String(round(percent * 100)) + "%)", 120 + width + 5, y, 50, 25, Justification::centredLeft);

            g.setColour(MidiListModel::Scoring::getColour(key));
            g.fillRect(120, y, width, 25);

            y += 30;
        }
    }

private:
    MidiListModel& model;
    MidiHeroSettings& settings;
};
