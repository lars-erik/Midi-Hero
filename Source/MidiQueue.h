#pragma once
#include "Global.h"
#include "TimedMidiMessage.h"

class MidiQueue
{
public:
    MidiQueue(int queueSize) :
        fifo(queueSize),
        messages(queueSize)
    { }
    MidiQueue()
        : MidiQueue(1 << 14)
    { }

    size_t size() const { return fifo.getNumReady(); }

    void push(const MidiBuffer& buffer, const shared_ptr<AudioPlayHead::PositionInfo> position, const double sampleRate)
    {
        for (const auto metadata : buffer)
            fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = make_shared<TimedMidiMessage>(metadata.getMessage(), position, sampleRate); });
    }

    void push(MidiMessage& message, const shared_ptr<AudioPlayHead::PositionInfo> position, const double sampleRate)
    {
        fifo.write(1).forEach([&](int dest) { messages[(size_t)dest] = make_shared<TimedMidiMessage>(std::move(message), position, sampleRate); });
    }

    template <typename OutputIt>
    void pop(OutputIt out)
    {
        fifo.read(fifo.getNumReady()).forEach([&](int source) { *out++ = std::move(messages[static_cast<size_t>(source)]); });
    }

private:
    AbstractFifo fifo;
    vector<shared_ptr<TimedMidiMessage>> messages;
};

