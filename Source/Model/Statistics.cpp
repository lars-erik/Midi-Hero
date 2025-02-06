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
    totalNotes++;
    int diff = msg->getPpqDiffInMs();
    totalMs += diff;
    maxMs = abs(diff) > abs(maxMs) ? diff : maxMs;
    minMs = totalNotes == 1 || abs(diff) < abs(minMs) ? diff : minMs;
    avgMs = static_cast<double>(totalMs) / max(static_cast<double>(totalNotes), 1.0);
    scoreCounts[msg->getScore().getScoreName()].accumulate(msg);
}

void Statistics::clear()
{
    totalNotes = 0;
    totalMs = 0;
    avgMs = 0;
    minMs = 0;
    maxMs = 0;
    scoreCounts = scoreCountsTemplate;
}

map<string, Statistics::ScoreCount> Statistics::getCounts() const
{
    return scoreCounts;
}

double Statistics::getAvgMs() const
{
    return avgMs;
}

int Statistics::getMaxMs() const
{
    return maxMs;
}

int Statistics::getMinMs() const
{
    return minMs;
}