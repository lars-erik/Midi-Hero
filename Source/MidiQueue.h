#pragma once
#include "Global.h"
#include "TimedMidiMessage.h"

class MidiQueue
{
public:
    void push(const MidiBuffer& buffer, const AudioPlayHead::PositionInfo& position, const double sampleRate)
    {
        for (const auto metadata : buffer)
            fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = TimedMidiMessage(metadata.getMessage(), position, sampleRate); });
    }

    void push(const MidiMessage& message, const AudioPlayHead::PositionInfo& position, const double sampleRate)
    {
        fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = TimedMidiMessage(message, position, sampleRate); });
    }

    template <typename OutputIt>
    void pop(OutputIt out)
    {
        fifo.read(fifo.getNumReady()).forEach([&](int source) { *out++ = messages[static_cast<size_t>(source)]; });
    }

private:
    static constexpr auto queueSize = 1 << 14;
    AbstractFifo fifo{ queueSize };
    std::vector<TimedMidiMessage> messages = std::vector<TimedMidiMessage>(queueSize);
};

