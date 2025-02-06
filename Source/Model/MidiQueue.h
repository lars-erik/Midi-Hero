#pragma once
#include "Global.h"
#include "MidiHeroSettings.h"
#include "TimedMidiMessage.h"

class MidiQueue
{
public:
    MidiQueue(int queueSize, const shared_ptr<MidiHeroSettings>& settings) :
        fifo(queueSize),
        messages(queueSize),
        settings(settings)
    { }
    MidiQueue(const shared_ptr<MidiHeroSettings>& settings)
        : MidiQueue(1 << 14, settings)
    { }

    size_t size() const { return fifo.getNumReady(); }

    void push(const MidiBuffer& buffer, const shared_ptr<AudioPlayHead::PositionInfo>& position, const shared_ptr<MidiHeroSettings>& settings, const double sampleRate)
    {
        for (const auto metadata : buffer)
            fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = make_shared<TimedMidiMessage>(metadata.getMessage(), position, settings, sampleRate); });
    }

    void push(MidiMessage& message, const shared_ptr<AudioPlayHead::PositionInfo>& position, const shared_ptr<MidiHeroSettings>& settings, const double sampleRate)
    {
        fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = make_shared<TimedMidiMessage>(std::move(message), position, settings, sampleRate); });
    }

    template <typename OutputIt>
    void pop(OutputIt out)
    {
        fifo.read(fifo.getNumReady()).forEach([&](int source) { *out++ = std::move(messages[static_cast<size_t>(source)]); });
    }

private:
    AbstractFifo fifo;
    vector<shared_ptr<TimedMidiMessage>> messages;
    const shared_ptr<MidiHeroSettings> settings;
};

