#pragma once
#include "Global.h"
#include "TimedMidiMessage.h"

class Statistics
{
public:
    struct ScoreCount
    {
        int total;
        int early;
        int late;
        int exact;

        void accumulate(shared_ptr<TimedMidiMessage> const& msg);

        double ratioOf(int totalNotes) const;
        double earlyRatio() const;
        double exactRatio() const;
        double lateRatio() const;
    };

    Statistics() = default;

    void clear();

    void accumulate(shared_ptr<TimedMidiMessage> const& msg);

    map<string, ScoreCount> getCounts();

private:
    const map<string, ScoreCount> scoreCountsTemplate = {
        {"Perfect", ScoreCount() },
        {"Great", ScoreCount() },
        {"Good", ScoreCount() },
        {"Off", ScoreCount() },
        {"Bad", ScoreCount() }
    };

    map<string, ScoreCount> scoreCounts = scoreCountsTemplate;

    int total;
};

