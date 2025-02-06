#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "Global.h"
#include "MidiListModel.h"
#include "TimedMidiMessage.h"

// Leave here to include in tests using TestData
#include "CsvFiles.h"
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

inline std::vector<shared_ptr<TimedMidiMessage>> transformToModel(std::vector<MidiData>& data, shared_ptr<MidiHeroSettings> const& settings, double bpm, double sampleRate)
{
    std::vector<shared_ptr<TimedMidiMessage>> messages;

    AudioPlayHead::TimeSignature timeSignature;
    timeSignature.numerator = 4;
    timeSignature.denominator = 4;

    ranges::transform(data, 
      std::back_inserter(messages),
      [&](const MidiData& item)
      {
          auto position = AudioPlayHead::PositionInfo();
          position.setBpm(bpm);
          position.setPpqPosition(item.ppqPosition);
          position.setPpqPositionOfLastBarStart(item.barPpqPosition);
          position.setTimeInSeconds(item.timeInSeconds);
          position.setTimeSignature(timeSignature);

          auto posPtr = make_shared<AudioPlayHead::PositionInfo>(position);

          return make_shared<TimedMidiMessage>(
              MidiMessage(item.byte1, item.byte2, item.byte3, item.timeStamp),
              posPtr,
              settings,
              sampleRate
          );
      }
    );

    return messages;
}

inline MidiListModel getTestData(const string& csvData, shared_ptr<MidiHeroSettings> const& settings, int bpm = 120, int sampleRate = 44100)
{
    auto midiData = readCsvFile(csvData);
    auto model = transformToModel(midiData, settings, bpm, sampleRate);
    return MidiListModel(model, settings);
}

inline MidiListModel getTestData(const string& csvData, int bpm = 120, int sampleRate = 44100)
{
    auto settings = createDefaultSettings();
    return getTestData(csvData, settings, bpm, sampleRate);
}
