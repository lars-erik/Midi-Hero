#include "MidiListModel.h"

MidiListModel::MidiListModel(const std::vector<shared_ptr<TimedMidiMessage>>& messages, const shared_ptr<MidiHeroSettings>& settings) :
    settings(settings),
    messages(messages)
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

MidiListModel::MidiListModel(const shared_ptr<MidiHeroSettings>& settings)
    : MidiListModel(vector<shared_ptr<TimedMidiMessage>>(0), settings)
{
}

const TimedMidiMessage& MidiListModel::operator[] (size_t ind) const { return *messages[ind]; }

void MidiListModel::recalculateScores()
{
    scoreCounts = scoreCountTemplate;
    score = accumulate(std::begin(notes), std::end(notes), Scoring(), [&](Scoring rt, shared_ptr<TimedMidiMessage> const& m)
        {
            scoreCounts[m->getScore().getScoreName()]++;
            return rt + m->getScore();
        });
}

void MidiListModel::addMessages(MidiQueue& queue)
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

void MidiListModel::processMessage(shared_ptr<TimedMidiMessage> const& msg)
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

void MidiListModel::clear()
{
    messages.clear();
    notes.clear();
    newNotes.clear();
    score = Scoring();
    scoreCounts = scoreCountTemplate;
    NoteCount.setValue(0);
}

size_t MidiListModel::size() const { return messages.size(); }

vector<shared_ptr<TimedMidiMessage>> MidiListModel::getMessages() const
{
    return messages;
}

vector<shared_ptr<TimedMidiMessage>> MidiListModel::getNewNotes() const
{
    return newNotes;
}

vector<shared_ptr<TimedMidiMessage>> MidiListModel::getNotes() const
{
    return notes;
}

Scoring MidiListModel::getBatchScore() const
{
    return batchScore;
}

Scoring MidiListModel::getScore() const
{
    return score;
}

std::map<std::string, int> MidiListModel::getScoreCounts() const
{
    return scoreCounts;
}

shared_ptr<MidiHeroSettings> const& MidiListModel::getSettings() const { return settings; }