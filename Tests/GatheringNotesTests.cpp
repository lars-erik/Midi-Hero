#pragma once
#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include "JuceHeader.h"

#include <ApprovalTests/Approvals.h>

#include "PluginProcessor.h"
#include "TestUtils.h"
#include "catch2/catch_all.hpp"

class DummyPlayHead : public AudioPlayHead
{
public:
    DummyPlayHead(double bpm)
        : position(PositionInfoBuilder()
            .withBpm(bpm)
            .withTimeInSeconds(0)
            .withIsPlaying(false)
            .build())
    {
    }

    PositionInfo& getMutablePosition() { return position; }
    Optional<PositionInfo> getPosition() const override { return position; };
    bool canControlTransport() override { return true; };
    void transportPlay(bool shouldStartPlaying) override {};
    void transportRecord(bool shouldStartRecording) override {};
    void transportRewind() override {};

    PositionInfo position;
};

struct ProcessorFixture {
    ProcessorFixture() :
        p(false, 32),
        ph(120)
    {
        p.setRateAndBufferSizeDetails(48'000, 1024);
        p.setPlayHead(&ph);
    }

    MidiHeroAudioProcessor p;
    MidiBuffer b;
    DummyPlayHead ph;

    void bufferAndProcess()
    {
        b.clear();
        b.addEvent(MidiMessage::noteOn (1, 0x48, 1.0f), 01); // C on
        b.addEvent(MidiMessage::noteOn (1, 0x4c, 1.0f), 10); // E on
        b.addEvent(MidiMessage::noteOff(1, 0x48, 1.0f), 13); // C off
        b.addEvent(MidiMessage::noteOn (1, 0x4f, 1.0f), 20); // G on
        b.addEvent(MidiMessage::noteOff(1, 0x4c, 1.0f), 70); // E off
        b.addEvent(MidiMessage::noteOff(1, 0x4f, 1.0f), 80); // G off
        p.process(b);
        while (p.hasQueuedItems())
            p.timerCallback();
    };
};

TEST_CASE_METHOD(ProcessorFixture, "Buffer gets transfered to model on timer")
{
    const auto unless = "unless not playing";
    SECTION(unless)
    {
        cout << Catch::getResultCapture().getCurrentTestName() << " " << unless << endl;

        bufferAndProcess();
        REQUIRE(p.model.getNoteCount() == 0);
        REQUIRE(p.model.size() == 0);
    }

    const auto whenPlaying = "when playing";
    SECTION(whenPlaying)
    {
        cout << Catch::getResultCapture().getCurrentTestName() << " " << whenPlaying << endl;
        ph.getMutablePosition().setIsPlaying(true);
        p.process(b);
        p.timerCallback();

        bufferAndProcess();
        REQUIRE(p.model.getNoteCount() == 3);
        REQUIRE(p.model.size() == 6);

        bufferAndProcess();
        REQUIRE(p.model.getNoteCount() == 6);

        ph.getMutablePosition().setIsPlaying(false);
        bufferAndProcess();

        REQUIRE(p.model.getNoteCount() == 6);
    }
}