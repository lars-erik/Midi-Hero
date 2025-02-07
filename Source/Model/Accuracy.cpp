#include "Accuracy.h"
#include "TimedMidiMessage.h"

Accuracy::Accuracy(shared_ptr<const TimedMidiMessage> message) :
    message(message),
    settings(message->settings)
{
}

Accuracy::Accuracy(int ms, shared_ptr<MidiHeroSettings> const& settings) :
    message(nullptr),
    settings(settings),
    ms(ms)
{
}

Accuracy Accuracy::operator+(Accuracy const& other)
{
    return
    {
        getMs() + other.getMs(),
        settings == nullptr
            ? other.settings
            : settings
    };
}

Accuracy Accuracy::operator+=(Accuracy const& other)
{
    if (message != nullptr)
    {
        ms = message->getPpqDiffInMs();
        message = nullptr;
    }
    if (settings == nullptr)
    {
        settings = other.settings;
    }
    ms += other.getMs();
    return *this;
}

Accuracy Accuracy::operator/(int divisor)
{
    int quotient = static_cast<int>(ceil(static_cast<double>(getMs()) / static_cast<double>(divisor)));
    return { quotient, settings };
}

bool Accuracy::isEmpty() const
{
    return settings == nullptr;
}

int Accuracy::getMs() const
{
    int subjMs;
    if (message != nullptr)
    {
        subjMs = message->getPpqDiffInMs();
    }
    else
    {
        subjMs = ms;
    }
    return subjMs;
}

string Accuracy::getDescription() const
{
    if (settings == nullptr) return "N/A";

    auto subjMs = getMs();
    auto& timing = settings->getTiming();

    if (subjMs < -timing.getOffMs()) return "Way too early";
    if (subjMs < -timing.getGoodMs()) return "Too early";
    if (subjMs < -timing.getGreatMs()) return "Early";
    if (subjMs < -timing.getPerfectMs()) return "Slightly early";

    if (subjMs > timing.getOffMs()) return "Way too late";
    if (subjMs > timing.getGoodMs()) return "Too late";
    if (subjMs > timing.getGreatMs()) return "Late";
    if (subjMs > timing.getPerfectMs()) return "Slightly late";

    return "Exact";
}

string Accuracy::toString() const
{
    return to_string(getMs()) + " (" + getDescription() + ")";
}

ostream& operator<<(ostream& os, Accuracy const& acc)
{
    return os << acc.toString();
}
