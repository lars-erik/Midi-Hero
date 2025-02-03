#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>

#include "PluginProcessor.h"
#include "catch2/catch_all.hpp"

class DummyPlayHead : public AudioPlayHead
{
public:
    DummyPlayHead(double bpm)
    {
        position.setBpm(bpm);
        position.setTimeInSeconds(0);
        position.setIsPlaying(false);
    }

    ~DummyPlayHead() override = default;
    PositionInfo& getMutablePosition() { return position; }
    Optional<PositionInfo> getPosition() const override { return position; };
    bool canControlTransport() override { return true; };
    void transportPlay(bool shouldStartPlaying) override {};
    void transportRecord(bool shouldStartRecording) override {};
    void transportRewind() override {};

    PositionInfo position;
};

TEST_CASE("Buffer gets transfered to model on timer")
{
    cout << "Start of testcase" << endl;

    MidiHeroAudioProcessor p{false, 32};
    MidiBuffer b;
    DummyPlayHead ph{120};
    
    p.setRateAndBufferSizeDetails(48'000, 1024);
    p.setPlayHead(&ph);

    auto bufferAndProcess = [&]()
    {
        b.addEvent(MidiMessage(0x90, 0x48, 0x7f, 0), 0);
        b.addEvent(MidiMessage(0x90, 0x4c, 0x7f, 0), 10);
        b.addEvent(MidiMessage(0x90, 0x4f, 0x7f, 0), 20);
        b.addEvent(MidiMessage(0x80, 0x48, 0x7f, 0), 60);
        b.addEvent(MidiMessage(0x80, 0x48, 0x7f, 0), 70);
        b.addEvent(MidiMessage(0x80, 0x48, 0x7f, 0), 80);
        p.process(b);
        while(p.hasQueuedItems())
            p.timerCallback();
    };

    SECTION("unless not playing")
    {
        bufferAndProcess();
        REQUIRE(p.model.getNoteCount() == 0);
        REQUIRE(p.model.size() == 0);
    }

    SECTION("when playing")
    {
        ph.getMutablePosition().setIsPlaying(true);
        p.process(b);
        p.timerCallback();
        bufferAndProcess();
        REQUIRE(p.model.getNoteCount() == 3);
        REQUIRE(p.model.size() == 6);
    }
}