#include "FmodEvent.h"

#include "Engine/Engine/Engine.h"
#include "FlaxFmod/FmodAudio.h"
#include "FlaxFmod/FmodAudioSystem.h"

float FmodEvent::GetLength() const
{
    if (!Engine::IsPlayMode())
        return -1.0f;
    return FmodAudio::GetAudioSystem()->GetEventLength(Path);   
}
