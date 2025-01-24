#pragma once

#include "Global.h"
#include "MidiHeroSettings.h"
#include "MidiListModel.h"
#include "TimedMidiMessage.h"

//==============================================================================
class MidiTable final : public Component,
    private TableListBoxModel
{
public:
    MidiTable(MidiListModel& m, MidiHeroSettings& settings) :
        messages(m),
        settings(settings)
    {
        addAndMakeVisible(table);

        filteredMessages = messages.getNotes();

        table.setModel(this);
        
        table.setHeader([&]
            {
                auto header = std::make_unique<TableHeaderComponent>();
                header->addColumn("Message", messageColumn, 100, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Bar", barColumn, 50, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("PPQ", ppqColumn, 80, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Position", adjustedColumn, 80, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("MS Diff", diffColumn, 100, 60, -1, TableHeaderComponent::notSortable);
                header->addColumn("Int. PPQ", intendedPpqColumn, 80, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Intended", intendedColumn, 80, 30, -1, TableHeaderComponent::notSortable);
                header->addColumn("Score", scoreColumn, 100, -1, TableHeaderComponent::notSortable);
                header->addColumn("Data", dataColumn, 200, 30, -1, TableHeaderComponent::notSortable);
                return header;
            }());

        messages.observeNoteCount(&noteCountObserver, [&](int) { updateTable(); });
        settings.observeDivisionLevel(&divisionLevelObserver, [&](int) { updateTable(); });

    }

    ~MidiTable() override
    {
        messages.stopObserveNoteCount(&noteCountObserver);
        settings.stopObserveDivisionLevel(&divisionLevelObserver);
    };

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
    void updateTable()
    {
        filteredMessages = messages.getNotes();
        table.updateContent();
    }

    enum
    {
        messageColumn = 1,
        barColumn,
        ppqColumn,
        adjustedColumn,
        diffColumn,
        intendedPpqColumn,
        intendedColumn,
        scoreColumn,
        channelColumn,
        dataColumn,
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

        const auto index = filteredMessages.size() - 1 - rowNumber;
        const auto message = filteredMessages[static_cast<int64>(index)];

        const int divisionLevel = settings.getDivisionLevel();

        return new Label({}, [&]
            {
                switch (columnId)
                {
                case messageColumn:     return getEventString(message.message);
                case barColumn:         return String(message.position.getPpqPositionOfLastBarStart().orFallback(0));
                case ppqColumn:         return String(message.position.getPpqPosition().orFallback(0));
                case adjustedColumn:    return String(message.getPositionFormatted());
                case intendedPpqColumn: return String(message.getIntendedPosition(divisionLevel));
                case intendedColumn:    return String(message.getIntendedPositionFormatted(divisionLevel));
                case diffColumn:        return String(message.getPpqDiffInMs(divisionLevel));
                case channelColumn:     return String(message.message.getChannel());
                case dataColumn:        return getDataString(message.message);
                case scoreColumn:       return String(message.getScoreName(divisionLevel)); 
                default:                break;
                }

                jassertfalse;
                return String();
            }());
    }

    MidiListModel& messages;
    MidiHeroSettings& settings;
    std::vector<TimedMidiMessage> filteredMessages;
    TableListBox table;

    Observer<int> noteCountObserver;
    Observer<int> divisionLevelObserver;
};