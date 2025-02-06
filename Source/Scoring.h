#pragma once
#include "Global.h"

struct Scoring
{
    static vector<double> scores;

    static vector<string> keys;

    static vector<Colour> colours;

    long notes;
    double score;
    double average;

    Scoring(long notes, double score);

    Scoring(double score);

    Scoring() = default;

    string getScoreName() const;

    Colour getColour() const;

    static Colour getColour(const string& scoreName);

    friend bool operator==(const Scoring& lhs, const Scoring& rhs);

    friend bool operator!=(const Scoring& lhs, const Scoring& rhs);

    Scoring operator+(const Scoring& other) const;

    Scoring& operator+=(const Scoring& other);

private:
    long index;

    void calcAverage();

    void findIndex();
};