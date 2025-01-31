#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "Global.h"
#include "TimedMidiMessage.h"

struct MidiData {
    double timeInSeconds;
    double ppqPosition;
    double barPpqPosition;
    int byte1;
    int byte2;
    int byte3;
    double timeStamp;
};

inline std::vector<TimedMidiMessage> transformToModel(std::vector<MidiData>& data, double bpm, double sampleRate)
{
    std::vector<TimedMidiMessage> messages;

    AudioPlayHead::TimeSignature timeSignature;
    timeSignature.numerator = 4;
    timeSignature.denominator = 4;

    std::transform(
        data.begin(), 
        data.end(), 
        std::back_inserter(messages),
        [sampleRate, bpm, timeSignature](const MidiData& data)
        {
            auto position = AudioPlayHead::PositionInfo();
            position.setBpm(bpm);
            position.setPpqPosition(data.ppqPosition);
            position.setPpqPositionOfLastBarStart(data.barPpqPosition);
            position.setTimeInSeconds(data.timeInSeconds);
            position.setTimeSignature(timeSignature);
            return TimedMidiMessage(
                MidiMessage(data.byte1, data.byte2, data.byte3, data.timeStamp),
                position,
                sampleRate
            );
        }
    );

    return messages;
}

inline std::vector<MidiData> readCsvFile(const string& csvData) {
    std::vector<MidiData> midiDataList;

    std::istringstream dataStream(csvData);
    std::string line;
    bool isHeader = true;

    // Read file line by line
    while (std::getline(dataStream, line)) {
        if (isHeader) {
            // Skip the header row
            isHeader = false;
            continue;
        }

        std::stringstream lineStream(line);
        std::string cell;
        MidiData midiData;

        // Parse each column
        std::getline(lineStream, cell, ';');
        midiData.timeInSeconds = std::stod(cell);

        std::getline(lineStream, cell, ';');
        midiData.ppqPosition = std::stod(cell);

        std::getline(lineStream, cell, ';');
        midiData.barPpqPosition = std::stod(cell);

        std::getline(lineStream, cell, ';');
        midiData.byte1 = std::stoi(cell, nullptr, 16);

        std::getline(lineStream, cell, ';');
        midiData.byte2 = std::stoi(cell, nullptr, 16);

        std::getline(lineStream, cell, ';');
        midiData.byte3 = std::stoi(cell, nullptr, 16);

        std::getline(lineStream, cell, ';');
        midiData.timeStamp = std::stod(cell);

        midiDataList.push_back(midiData);
    }

    return midiDataList;
}

inline std::vector<TimedMidiMessage> getTestData(const string& csvData, int bpm = 120, int sampleRate = 44100)
{
    auto midiData = readCsvFile(csvData);
    auto model = transformToModel(midiData, bpm, sampleRate);
    return model;
}
