#pragma once
#include "Global.h"
#include "MidiQueue.h"
#include "Observer.h"
#include "MidiHeroSettings.h"
#include "Scoring.h"
#include "Statistics.h"
#include "TimedMidiMessage.h"

// Stores the last N messages. Safe to access from the message thread only.
class MidiListModel
{
public:
    explicit MidiListModel(const std::vector<shared_ptr<TimedMidiMessage>>& messages, const shared_ptr<MidiHeroSettings>& settings);

    MidiListModel(const shared_ptr<MidiHeroSettings>& settings);

    void recalculateScores();

    void addMessages(MidiQueue& queue);

    void clear();

    const TimedMidiMessage& operator[] (size_t ind) const;

    size_t size() const;

    vector<shared_ptr<TimedMidiMessage>> getMessages() const;

    vector<shared_ptr<TimedMidiMessage>> getNewNotes() const;

    vector<shared_ptr<TimedMidiMessage>> getNotes() const;

    Scoring getBatchScore() const;

    Scoring getScore() const;

    Statistics getStatistics() const;

    std::map<std::string, int> getScoreCounts() const;

    shared_ptr<MidiHeroSettings> const& getSettings() const;

    READONLY_OBSERVABLE(int, NoteCount)

    READONLY_OBSERVABLE(bool, IsPlaying)

private:
    static bool isNoteOn(shared_ptr<TimedMidiMessage> const& msg) { return msg->message.isNoteOn(); }

    void processMessage(shared_ptr<TimedMidiMessage> const& msg);

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
    Statistics stats = Statistics();
};