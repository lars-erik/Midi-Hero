#pragma once
#include "Global.h"
#include "Observer.h"
#include "TimedMidiMessage.h"

// Stores the last N messages. Safe to access from the message thread only.
class MidiListModel
{
public:
    MidiListModel() = default;

    explicit MidiListModel(const std::vector<TimedMidiMessage>& messages)
        : messages(messages)
    {
    }

    template <typename It>
    void addMessages(It begin, It end)
    {
        if (begin == end)
            return;

        const auto numNewMessages = (int)std::distance(begin, end);
        const auto numToAdd = juce::jmin(numToStore, numNewMessages);
        const auto numToRemove = jmax(0, (int)messages.size() + numToAdd - numToStore);
        messages.erase(messages.begin(), std::next(messages.begin(), numToRemove));
        messages.insert(messages.end(), std::prev(end, numToAdd), end);

        newNotes.clear();
        copy_if(begin, end, std::back_inserter(newNotes), [](const TimedMidiMessage& m) { return m.message.isNoteOn(); });

        bool hasStartStop = false;
        bool isStart = false;
        for_each(begin, end, [&hasStartStop, &isStart](const TimedMidiMessage& msg)
        {
            if (msg.message.isMidiStart())
            {
                hasStartStop = true;
                isStart = true;
            }
            if (msg.message.isMidiStop())
            {
                hasStartStop = true;
                isStart = false;
            }
        });
        if (hasStartStop)
        {
            // TODO: If setting & bar 0?
            if (hasStartStop && isStart)
            {
                messages.clear();
            }

            IsPlaying.setValue(isStart);
        }

        NoteCount.setValue(static_cast<int>(messages.size()));
    }

    void clear()
    {
        messages.clear();

        NoteCount.setValue(static_cast<int>(messages.size()));
    }

    const TimedMidiMessage& operator[] (size_t ind) const { return messages[ind]; }

    size_t size() const { return messages.size(); }

    // Provide const iterators for read-only access
    std::vector<TimedMidiMessage>::const_iterator begin() const {
        return messages.cbegin();
    }

    std::vector<TimedMidiMessage>::const_iterator end() const {
        return messages.cend();
    }

    template <typename Predicate>
    std::vector<TimedMidiMessage> filterMessages(Predicate predicate)
    {
        std::vector<TimedMidiMessage> notes;
        copy_if(
            messages.begin(),
            messages.end(),
            std::back_inserter(notes),
            predicate
        );
        return notes;
    }

    std::vector<TimedMidiMessage> getNewNotes()
    {
        return newNotes;
    }

    struct Scoring
    {
        inline static std::vector<std::string> keys = {
            "Perfect", "Great", "Good", "Off", "Bad"
        };

        long notes;
        double score;
        double total;

        Scoring() = default;

        Scoring(long notes, double score, double total)
            : notes(notes),
            score(score),
            total(total)
        {
        }

        std::string getScoreName() const
        {
            if (total >= .9) return "Perfect";
            if (total >= .8) return "Great";
            if (total >= .6) return "Good";
            if (total >= .25) return "Off";
            return "Bad";
        }

        Colour getColour() const
        {
            return getColour(getScoreName());
        }

        static Colour getColour(const std::string& scoreName)
        {
            static const std::unordered_map<std::string, Colour> colors = {
                {"Perfect", Colours::green },
                {"Great", Colours::lightgreen },
                {"Good", Colours::greenyellow },
                {"Off", Colours::orange },
                {"Bad", Colours::red }
            };

            return colors.at(scoreName);
        }
    };

    static Scoring getScore(std::vector<TimedMidiMessage> notes, int divisionLevel)
    {
        vector<double> scores;
        transform(notes.begin(), notes.end(), back_inserter(scores), [divisionLevel](const TimedMidiMessage& m) { return m.getScore(divisionLevel); });
        const long totalNotes = static_cast<long>(notes.size());
        const double score = std::accumulate(scores.begin(), scores.end(), 0.0);
        const double totalScore = round(score / totalNotes * 100) / 100;
        return { totalNotes, score, totalScore };
    }

    std::vector<TimedMidiMessage> getNotes()
    {
        return filterMessages([&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
    }

    Scoring getScore(const int divisionLevel)
    {
        auto notes = getNotes();
        return getScore(notes, divisionLevel);
    }

    std::map<std::string, int> getScoreCounts(const int divisionLevel)
    {
        auto notes = getNotes();

        std::map<std::string, int> scores = {
            {"Perfect", 0 },
            {"Great", 0 },
            {"Good", 0 },
            {"Off", 0 },
            {"Bad", 0 }
        };

        for (TimedMidiMessage& m : notes)
        {
            scores[m.getScoreName(divisionLevel)]++;
        }

        return scores;
    }

    READONLY_OBSERVABLE(int, NoteCount)

    READONLY_OBSERVABLE(bool, IsPlaying)

private:
    static constexpr auto numToStore = 1000;
    std::vector<TimedMidiMessage> messages;
    std::vector<TimedMidiMessage> newNotes;
};