#pragma once
#include "JuceHeader.h"
using namespace juce;

struct PositionInfoBuilder
{
    AudioPlayHead::PositionInfo positionInfo;

    PositionInfoBuilder& withBpm(double bpm)
    {
        positionInfo.setBpm(bpm);
        return *this;
    }

    PositionInfoBuilder& withIsPlaying(bool isPlaying) {
        positionInfo.setIsPlaying(isPlaying);
        return *this;
    }

    PositionInfoBuilder& withTimeInSeconds(double timeInSeconds) {
        positionInfo.setTimeInSeconds(timeInSeconds);
        return *this;
    }

    AudioPlayHead::PositionInfo build() const {
        return positionInfo;
    }
};
