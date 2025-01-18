#pragma once

#include <iterator>
#include <cmath>

using namespace juce;

struct TimedMidiMessage
{
    MidiMessage message;
    AudioPlayHead::PositionInfo position;
    double sampleRate;

    TimedMidiMessage() = default;

    TimedMidiMessage(MidiMessage message, const AudioPlayHead::PositionInfo& position, const double sampleRate)
        : message(std::move(message)),
          position(position),
          sampleRate(sampleRate)
    {
    }

    double getAdjustedPpqPosition() const
    {
        double samplesPerPpq = sampleRate * 60 / position.getBpm().orFallback(60);
        double adjustedPpqPosition = message.getTimeStamp() / samplesPerPpq + position.getPpqPosition().orFallback(0);
        double result = roundToDecimals(adjustedPpqPosition, 3);
        return result;
    }

    double getIntendedPpqPosition(int divisionLevel) const {
        double step = 1.0 / divisionLevel; // Grid step size
        double nearest = std::round(getAdjustedPpqPosition() / step) * step;
        return nearest;
    }

    double getPpqDiff(int divisionLevel) const
    {
        return getAdjustedPpqPosition() - getIntendedPpqPosition(divisionLevel);
    }

    int getPpqDiffInMs(int divisionLevel) const
    {
        double secPerQuarterNote = 60 / position.getBpm().orFallback(60); // 0.5 in 120
        double diff = getPpqDiff(divisionLevel); // ratio of 0.5? (yes)
        double diffInSeconds = diff * secPerQuarterNote;
        int result = round(diffInSeconds * 1000);
        return result;
    }

    double getScore(int divisionLevel) const
    {
        const int diff = abs(getPpqDiffInMs(divisionLevel));
        if (diff < 10) return 1;
        if (diff < 20) return .75;
        if (diff < 30) return .5;
        if (diff < 40) return .25;
        return 0;
    }

    std::string getScoreName(int divisionLevel) const
    {
        const int diff = abs(getPpqDiffInMs(divisionLevel));
        if (diff < 10) return "Perfect";
        if (diff < 20) return "Good";
        if (diff < 30) return "Off";
        if (diff < 40) return "Bad";
        return "Audible";
    }

private:
    static double roundToDecimals(double value, int decimals) {
        double scale = std::pow(10.0, decimals);
        return std::round(value * scale) / scale;
    }

};

class MidiQueue
{
public:
    void push(const MidiBuffer& buffer, const AudioPlayHead::PositionInfo& position, const double sampleRate)
    {
        for (const auto metadata : buffer)
            fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = TimedMidiMessage(metadata.getMessage(), position, sampleRate); });
    }

    void push(const MidiMessage& message, const AudioPlayHead::PositionInfo& position, const double sampleRate)
    {
        fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = TimedMidiMessage(message, position, sampleRate); });
    }

    template <typename OutputIt>
    void pop(OutputIt out)
    {
        fifo.read(fifo.getNumReady()).forEach([&](int source) { *out++ = messages[static_cast<size_t>(source)]; });
    }

private:
    static constexpr auto queueSize = 1 << 14;
    AbstractFifo fifo{ queueSize };
    std::vector<TimedMidiMessage> messages = std::vector<TimedMidiMessage>(queueSize);
};

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

        NullCheckedInvocation::invoke(onChange);
    }

    void addMessage(const TimedMidiMessage& message)
    {
        messages.push_back(message);

        NullCheckedInvocation::invoke(onChange);
    }

    void clear()
    {
        messages.clear();

        NullCheckedInvocation::invoke(onChange);
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

    struct Scoring
    {
        int notes;
        double score;
        double total;

        Scoring() = default;

        Scoring(int notes, double score, double total)
            : notes(notes),
              score(score),
              total(total)
        {
        }
    };

    Scoring score(int divisionLevel)
    {
        auto notes = filterMessages([&](const TimedMidiMessage& msg) { return msg.message.isNoteOn(); });
        std::vector<double> scores;
        std::transform(notes.begin(), notes.end(), std::back_inserter(scores), [divisionLevel](const TimedMidiMessage& m) { return m.getScore(divisionLevel); });
        double score = std::accumulate(scores.begin(), scores.end(), 0.0);
        double totalScore = round(score / notes.size() * 100) / 100;
        return Scoring(notes.size(), score, totalScore);
    }

    std::function<void()> onChange;

private:
    static constexpr auto numToStore = 1000;
    std::vector<TimedMidiMessage> messages;
};

//==============================================================================
class MidiTable final : public Component,
    private TableListBoxModel
{
public:
    MidiTable(MidiListModel& m)
        : messages(m)
    {
        addAndMakeVisible(table);

        filteredMessages = messages.filterMessages([](TimedMidiMessage& m) { return m.message.isNoteOn(); });

        table.setModel(this);
        //table.setClickingTogglesRowSelection(false);
        
        table.setHeader([&]
            {
                auto header = std::make_unique<TableHeaderComponent>();
                header->addColumn("Message", messageColumn, 200, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Bar", barColumn, 100, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Time", adjustedColumn, 100, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Data", dataColumn, 200, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Score", scoreColumn, 200, -1, TableHeaderComponent::notSortable);
                return header;
            }());

        messages.onChange = [&]
        {
            filteredMessages = messages.filterMessages([](TimedMidiMessage& m) { return m.message.isNoteOn(); });
            table.updateContent();
        };
    }

    ~MidiTable() override { messages.onChange = nullptr; }

    void resized() override { table.setBounds(getLocalBounds()); }

    static String getEventString(const MidiMessage& m)
    {
        if (m.isNoteOn())           return "Note on";
        if (m.isNoteOff())          return "Note off";
        if (m.isProgramChange())    return "Program change";
        if (m.isPitchWheel())       return "Pitch wheel";
        if (m.isAftertouch())       return "Aftertouch";
        if (m.isChannelPressure())  return "Channel pressure";
        if (m.isAllNotesOff())      return "All notes off";
        if (m.isAllSoundOff())      return "All sound off";
        if (m.isMetaEvent())        return "Meta event";
        if (m.isMidiStart())        return "MIDI Start";
        if (m.isMidiStop())         return "MIDI Stop";
        if (m.isMidiContinue())     return "MIDI Continue";

        if (m.isController())
        {
            const auto* name = MidiMessage::getControllerName(m.getControllerNumber());
            return "Controller " + (name == nullptr ? String(m.getControllerNumber()) : String(name));
        }

        return String::toHexString(m.getRawData(), m.getRawDataSize());
    }

    static String getDataString(const MidiMessage& m)
    {
        if (m.isNoteOn())           return MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + " Velocity " + String(m.getVelocity());
        if (m.isNoteOff())          return MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + " Velocity " + String(m.getVelocity());
        if (m.isProgramChange())    return String(m.getProgramChangeNumber());
        if (m.isPitchWheel())       return String(m.getPitchWheelValue());
        if (m.isAftertouch())       return MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + String(m.getAfterTouchValue());
        if (m.isChannelPressure())  return String(m.getChannelPressureValue());
        if (m.isController())       return String(m.getControllerValue());

        return {};
    }

private:
    enum
    {
        messageColumn = 1,
        timeColumn,
        barColumn,
        ppqColumn,
        adjustedColumn,
        channelColumn,
        dataColumn,
        scoreColumn
    };

    int getNumRows() override { return (int)filteredMessages.size(); }

    void paintRowBackground(Graphics&, int, int, int, bool) override {}
    void paintCell(Graphics&, int, int, int, int, bool)     override {}

    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool,
        Component* existingComponentToUpdate) override
    {
        delete existingComponentToUpdate;

        const auto index = (int)filteredMessages.size() - 1 - rowNumber;
        const auto message = filteredMessages[(size_t)index];


        return new Label({}, [&]
            {
                switch (columnId)
                {
                case messageColumn:  return getEventString(message.message);
                case timeColumn:     return String(message.message.getTimeStamp());
                case barColumn:      return String(message.position.getPpqPositionOfLastBarStart().orFallback(0));
                case ppqColumn:      return String(message.position.getPpqPosition().orFallback(0));
                case adjustedColumn: return String(message.getAdjustedPpqPosition());
                case channelColumn:  return String(message.message.getChannel());
                case dataColumn:     return getDataString(message.message);
                case scoreColumn:    return String(message.getScoreName(4)); // TODO: Configurable division level
                default:             break;
                }

                jassertfalse;
                return String();
            }());
    }

    MidiListModel& messages;
    std::vector<TimedMidiMessage> filteredMessages;
    TableListBox table;
};