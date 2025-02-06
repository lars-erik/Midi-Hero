#include "Scoring.h"

vector<double> Scoring::scores = {
    1, .9, .6, .25, 0
};

vector<string> Scoring::keys = {
    "Perfect", "Great", "Good", "Off", "Bad"
};

vector<Colour> Scoring::colours = {
    Colours::green, Colours::lightgreen, Colours::greenyellow, Colours::orange, Colours::red
};

Scoring::Scoring(long notes, double score) :
    notes(notes),
    score(score)
{
    calcAverage();
    findIndex();
}

Scoring::Scoring(double score) : Scoring(1, score) {}

string Scoring::getScoreName() const
{
    return keys[index];
}

Colour Scoring::getColour() const
{
    return colours[index];
}

Colour Scoring::getColour(const string& scoreName)
{
    auto it = find(begin(keys), end(keys), scoreName);
    long index = it - begin(keys);
    return colours[index];
}

bool operator==(const Scoring& lhs, const Scoring& rhs)
{
    return lhs.notes == rhs.notes
        && abs(lhs.score - rhs.score) < .0001
        && abs(lhs.average - rhs.average) < .0001;
}

bool operator!=(const Scoring& lhs, const Scoring& rhs)
{
    return !(lhs == rhs);
}

Scoring Scoring::operator+(const Scoring& other) const
{
    return { notes + other.notes, score + other.score };
}

Scoring& Scoring::operator+=(const Scoring& other) {
    notes += other.notes;
    score += other.score;
    calcAverage();
    findIndex();
    return *this;
}

void Scoring::calcAverage()
{
    average = round(score / notes * 100) / 100;
}

void Scoring::findIndex()
{
    const auto it = lower_bound(begin(scores), end(scores), average, greater<>());
    index = static_cast<long>(it - begin(scores));
}