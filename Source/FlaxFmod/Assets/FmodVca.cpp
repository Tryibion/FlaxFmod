#include "FmodVca.h"

#include "FlaxFmod/FmodAudio.h"

float FmodVca::GetVolume() const
{
    return FmodAudio::GetVCAVolume(Path);
}

void FmodVca::SetVolume(float volume) const
{
    FmodAudio::SetVCAVolume(Path, volume);
}
