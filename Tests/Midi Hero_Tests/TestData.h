#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "MidiUtils.h"

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
    std::transform(
        data.begin(), 
        data.end(), 
        std::back_inserter(messages),
        [sampleRate, bpm](const MidiData& data)
        {
            auto position = AudioPlayHead::PositionInfo();
            position.setBpm(bpm);
            position.setPpqPosition(data.ppqPosition);
            position.setPpqPositionOfLastBarStart(data.barPpqPosition);
            position.setTimeInSeconds(data.timeInSeconds);
            return TimedMidiMessage(
                MidiMessage(data.byte1, data.byte2, data.byte3, data.timeStamp),
                position,
                sampleRate
            );
        }
    );

    return messages;
}

inline std::string loadCsvFromResource(int resourceId, const wchar_t* resourceType) {
    // Find the resource in the executable
    HRSRC resourceHandle = FindResource(nullptr, MAKEINTRESOURCE(resourceId), resourceType);
    if (!resourceHandle) {
        throw std::runtime_error("Resource not found");
    }

    // Load the resource
    HGLOBAL resourceData = LoadResource(nullptr, resourceHandle);
    if (!resourceData) {
        throw std::runtime_error("Failed to load resource");
    }

    // Get a pointer to the resource data
    DWORD resourceSize = SizeofResource(nullptr, resourceHandle);
    const char* resourcePointer = static_cast<const char*>(LockResource(resourceData));

    if (!resourcePointer) {
        throw std::runtime_error("Failed to lock resource");
    }

    // Return the resource content as a string
    return std::string(resourcePointer, resourceSize);
}

inline std::vector<MidiData> readCsvFile(int resourceId, const wchar_t* resourceType) {
    std::vector<MidiData> midiDataList;

    std::string data = loadCsvFromResource(resourceId, resourceType);
    std::istringstream dataStream(data);
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
