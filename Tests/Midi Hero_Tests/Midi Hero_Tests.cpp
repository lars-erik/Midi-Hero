#include "JuceHeader.h"
#include "catch_amalgamated.hpp"
#include "MidiUtils.h"
#include "resource.h"
#include "TestUtils.h"

struct MidiData {
    double timeInSeconds;
    double ppqPosition;
    double barPpqPosition;
    int byte1;
    int byte2;
    int byte3;
    double timeStamp;
};
std::vector<MidiData> readCsvFile(int resourceId, const wchar_t* resourceType);

using namespace juce;

TEST_CASE("I'm alive", "[basic]")
{
    auto midiData = readCsvFile(IDR_CSV_FILE, L"CSV");
    auto first = midiData.at(0);
    auto second = midiData.at(1);

    REQUIRE(first.byte1 == 0xfa);
    REQUIRE(second.byte1 == 0xfc);
    REQUIRE(second.timeInSeconds == 1);
}


#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

std::string loadCsvFromResource(int resourceId, const wchar_t* resourceType) {
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

std::vector<MidiData> readCsvFile(int resourceId, const wchar_t* resourceType) {
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
