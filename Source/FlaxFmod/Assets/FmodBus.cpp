#include "FmodBus.h"

#include "FlaxFmod/FmodAudio.h"

float FmodBus::GetVolume() const
{
    return FmodAudio::GetBusVolume(Path);
}

void FmodBus::SetVolume(float volume) const
{
    FmodAudio::SetBusVolume(Path, volume);
}

bool FmodBus::GetMuted() const
{
    return FmodAudio::GetBusMute(Path);
}

void FmodBus::SetMuted(bool muted) const
{
    FmodAudio::SetBusMute(Path, muted);
}

bool FmodBus::GetPaused() const
{
    return FmodAudio::IsBusPaused(Path);
}

void FmodBus::SetPaused(bool paused) const
{
    FmodAudio::SetBusPaused(Path, paused);
}
