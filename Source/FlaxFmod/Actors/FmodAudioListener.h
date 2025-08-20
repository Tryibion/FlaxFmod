#pragma once

#include "Engine/Audio/AudioListener.h"
#include "Engine/Level/Actor.h"

API_CLASS(Attributes="ActorContextMenu(\"New/Audio/Fmod Audio Listener\"), ActorToolbox(\"Other\")")
class FLAXFMOD_API FmodAudioListener : public Actor
{
API_AUTO_SERIALIZATION();
DECLARE_SCENE_OBJECT(FmodAudioListener);

public:
    /// <summary>
    /// Gets the velocity of the audio listener.
    /// </summary>
    API_FUNCTION() Vector3 GetVelocity() const
    {
        return _velocity;
    }

private:
    Vector3 _previousPosition;
    Vector3 _velocity;

    // [Actor]
    void OnEnable() override;
    void Update();
    void OnDisable() override;
    void OnBeginPlay() override;
    void OnEndPlay() override;

    void OnTransformChanged() override;
};
