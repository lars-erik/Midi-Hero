#pragma once
#include "Global.h"
#include "MidiQueue.h"
#include "Observer.h"
#include "MidiHeroSettings.h"
#include "TimedMidiMessage.h"


// Stores the last N messages. Safe to access from the message thread only.
class MidiListModel
{
public:
    explicit MidiListModel(const std::vector<shared_ptr<TimedMidiMessage>>& messages, const shared_ptr<MidiHeroSettings>& settings) : 
        messages(messages),
        settings(settings)
    {
        settings->observeDivisionLevel(&divisionLevelObserver, [&](int) { recalculateScores(); });
        settings->getTiming().observeScale(&scalingObserver, [&](float) { recalculateScores(); });

        if (!messages.empty())
        {
            notes = filterMessages(isNoteOn);
            recalculateScores();
        }

        NoteCount.setValue(static_cast<int>(notes.size()));
    }

    MidiListModel(const shared_ptr<MidiHeroSettings>& settings)
        : MidiListModel(vector<shared_ptr<TimedMidiMessage>>(0), settings)
    {
    }

    void recalculateScores()
    {
        scoreCounts = scoreCountTemplate;
        score = accumulate(std::begin(notes), std::end(notes), Scoring(), [&](Scoring rt, shared_ptr<TimedMidiMessage> const& m)
        {
            scoreCounts[m->getScore().getScoreName()]++;
            return rt + m->getScore();
        });
    }

    void addMessages(MidiQueue& queue)
    {
        const auto numNewMessages = (int)queue.size();
        const auto numToAdd = juce::jmin(numToStore, numNewMessages);

        // This is here to cycle size around numToStore
        const auto numToRemove = jmax(0, (int)messages.size() + numToAdd - numToStore);
        messages.erase(messages.begin(), std::next(messages.begin(), numToRemove));

        // Transfer to messages
        queue.pop(back_inserter(messages));

        auto last = messages.end();
        auto first = last - numToAdd;

        newNotes.clear();
        batchScore = Scoring();
        for_each(first, last, [&](auto const& msg) { processMessage(msg); });

        int newCount = static_cast<int>(newNotes.size()) + static_cast<int>(NoteCount.getValue());
        NoteCount.setValue(newCount);
    }

    void clear()
    {
        messages.clear();
        notes.clear();
        newNotes.clear();
        score = Scoring();
        scoreCounts = scoreCountTemplate;
        NoteCount.setValue(0);
    }

    const TimedMidiMessage& operator[] (size_t ind) const { return *messages[ind]; }

    size_t size() const { return messages.size(); }

    vector<shared_ptr<TimedMidiMessage>> getMessages() const
    {
        return messages;
    }

    vector<shared_ptr<TimedMidiMessage>> getNewNotes() const
    {
        return newNotes;
    }

    vector<shared_ptr<TimedMidiMessage>> getNotes() const
    {
        return notes;
    }

    Scoring getBatchScore() const
    {
        return batchScore;
    }

    Scoring getScore() const
    {
        return score;
    }

    std::map<std::string, int> getScoreCounts() const
    {
        return scoreCounts;
    }

    shared_ptr<MidiHeroSettings> const& getSettings() const { return settings; }

    READONLY_OBSERVABLE(int, NoteCount)

    READONLY_OBSERVABLE(bool, IsPlaying)

private:
    static bool isNoteOn(shared_ptr<TimedMidiMessage> const& msg) { return msg->message.isNoteOn(); }

    void processMessage(shared_ptr<TimedMidiMessage> msg)
    {
        if (msg->message.isMidiStart())
        {
            IsPlaying.setValue(true);

            // TODO: If enabled &| is at bar 0
            clear();
        }
        if (msg->message.isMidiStop())
        {
            IsPlaying.setValue(false);
        }
        if (msg->message.isNoteOn())
        {
            notes.push_back(msg);
            newNotes.push_back(msg);
            auto scoring = msg->getScore();
            score += scoring;
            batchScore += scoring;
            scoreCounts[scoring.getScoreName()]++;
        }
    }

    template <typename Predicate>
    std::vector<shared_ptr<TimedMidiMessage>> filterMessages(Predicate predicate)
    {
        std::vector<shared_ptr<TimedMidiMessage>> notes;
        copy_if(
            messages.begin(),
            messages.end(),
            std::back_inserter(notes),
            predicate
        );
        return notes;
    }

    const map<string, int> scoreCountTemplate = {
        {"Perfect", 0 },
        {"Great", 0 },
        {"Good", 0 },
        {"Off", 0 },
        {"Bad", 0 }
    };

    map<string, int> scoreCounts = scoreCountTemplate;

    shared_ptr<MidiHeroSettings> settings;
    Observer<int> divisionLevelObserver;
    Observer<float> scalingObserver;

    static constexpr auto numToStore = 10'000;
    std::vector<shared_ptr<TimedMidiMessage>> messages;
    std::vector<shared_ptr<TimedMidiMessage>> newNotes;
    std::vector<shared_ptr<TimedMidiMessage>> notes;
    Scoring score = Scoring();
    Scoring batchScore = Scoring();
};