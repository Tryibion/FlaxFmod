#include "FmodAudioListener.h"

#include "Engine/Engine/Engine.h"
#include "Engine/Engine/Time.h"
#include "Engine/Level/Scene/Scene.h"
#include "FlaxFmod/FmodAudio.h"

FmodAudioListener::FmodAudioListener(const SpawnParams& params)
    : Actor(params)
    ,_velocity(Vector3::Zero)
    ,_previousPosition(Vector3::Zero)
{

}

void FmodAudioListener::OnEnable()
{
    Actor::OnEnable();

#if USE_EDITOR
    GetSceneRendering()->AddViewportIcon(this);
#endif

    // Code executed only during play mode.
    if (!Engine::IsPlayMode())
        return;

    FmodAudio::ActiveListener = this;
    FmodAudio::Listeners.AddUnique(this);
    GetScene()->Ticking.Update.AddTick<FmodAudioListener, &FmodAudioListener::Update>(this);
}

void FmodAudioListener::Update()
{
    // Update the velocity
    const Vector3 pos = GetPosition();
    const float dt = Time::Update.UnscaledDeltaTime.GetTotalSeconds();
    const auto prevVelocity = _velocity;
    _velocity = (pos - _previousPosition) / dt;
    _previousPosition = pos;
}

void FmodAudioListener::OnDisable()
{

#if USE_EDITOR
    GetSceneRendering()->RemoveViewportIcon(this);
#endif
    
    // Code executed only during play mode.
    if (Engine::IsPlayMode())
    {
        if (FmodAudio::ActiveListener == this)
            FmodAudio::ActiveListener = nullptr;
        
        FmodAudio::Listeners.Remove(this);
        GetScene()->Ticking.Update.RemoveTick(this);
    }

    Actor::OnDisable();
}

void FmodAudioListener::OnBeginPlay()
{
    Actor::OnBeginPlay();

    if (!Engine::IsPlayMode())
        return;
    
}

void FmodAudioListener::OnEndPlay()
{
    if (Engine::IsPlayMode())
    {
        
    }

    Actor::OnEndPlay();
}

void FmodAudioListener::OnTransformChanged()
{
    Actor::OnTransformChanged();

    _box = BoundingBox(_transform.Translation);
    _sphere = BoundingSphere(_transform.Translation, 0.0f);

    if (Engine::IsPlayMode())
    {
        
    }
}
