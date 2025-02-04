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
    MidiHeroAudioProcessor p{false, 32};
    MidiBuffer b;
    DummyPlayHead ph{120};
    
    p.setRateAndBufferSizeDetails(48'000, 1024);
    p.setPlayHead(&ph);

    auto bufferAndProcess = [&]()
    {
        b.clear();
        b.addEvent(MidiMessage(0x90, 0x48, 0x7f, 0), 01); // C on
        b.addEvent(MidiMessage(0x90, 0x4c, 0x7f, 0), 10); // E on
        b.addEvent(MidiMessage(0x80, 0x48, 0x7f, 0), 13); // C off
        b.addEvent(MidiMessage(0x90, 0x4f, 0x7f, 0), 20); // G on
        b.addEvent(MidiMessage(0x80, 0x4c, 0x7f, 0), 70); // E off
        b.addEvent(MidiMessage(0x80, 0x4f, 0x7f, 0), 80); // G off
        p.process(b);
        while(p.hasQueuedItems())
            p.timerCallback();
    };

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