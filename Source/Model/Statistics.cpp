#include "Statistics.h"

void Statistics::ScoreCount::accumulate(shared_ptr<TimedMidiMessage> const& msg)
{
    total++;
    int perfectMs = msg->settings->getTiming().getPerfectMs();
    if (msg->getPpqDiffInMs() < perfectMs * -1) early++;
    else if (msg->getPpqDiffInMs() > perfectMs) late++;
    else exact++;
}

double Statistics::ScoreCount::ratioOf(int totalNotes) const
{
    if (totalNotes == 0) return 0;
    return static_cast<double>(total) / static_cast<double>(totalNotes);
}

double Statistics::ScoreCount::earlyRatio() const
{
    if (total == 0) return 0;
    return static_cast<double>(early) / static_cast<double>(total);
}

double Statistics::ScoreCount::exactRatio() const
{
    if (total == 0) return 0;
    return static_cast<double>(exact) / static_cast<double>(total);
}

double Statistics::ScoreCount::lateRatio() const
{
    if (total == 0) return 0;
    return static_cast<double>(late) / static_cast<double>(total);
}

void Statistics::accumulate(shared_ptr<TimedMidiMessage> const& msg)
{
    scoreCounts[msg->getScore().getScoreName()].accumulate(msg);
}

void Statistics::clear()
{
    total = 0;
    scoreCounts = scoreCountsTemplate;
}

map<string, Statistics::ScoreCount> Statistics::getCounts()
{
    return scoreCounts;
}