#pragma once
#include "Global.h"
#include "MidiHeroSettings.h"
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

    Statistics(shared_ptr<MidiHeroSettings> const&);

    void clear();

    void accumulate(shared_ptr<TimedMidiMessage> const& msg);

    map<string, ScoreCount> getCounts() const;
    Accuracy getAvgAccuracy() const;
    Accuracy getMaxAccuracy() const;
    Accuracy getMinAccuracy() const;

private:
    const map<string, ScoreCount> scoreCountsTemplate = {
        {"Perfect", ScoreCount() },
        {"Great", ScoreCount() },
        {"Good", ScoreCount() },
        {"Off", ScoreCount() },
        {"Bad", ScoreCount() }
    };

    map<string, ScoreCount> scoreCounts = scoreCountsTemplate;

    shared_ptr<MidiHeroSettings> settings;

    int totalNotes = 0;
    int totalMs = 0;
    double avgMs = 0;
    int maxMs = 0;
    int minMs = 0;
};

