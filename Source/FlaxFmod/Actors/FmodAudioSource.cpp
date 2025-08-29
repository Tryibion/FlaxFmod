#include "FmodAudioSource.h"

#include "Engine/Engine/Engine.h"
#include "Engine/Engine/Time.h"
#include "Engine/Level/Scene/Scene.h"
#include "FlaxFmod/FmodAudio.h"
#include "FlaxFmod/FmodAudioSystem.h"

FmodAudioSource::FmodAudioSource(const SpawnParams& params)
    : Actor(params)
{
    Event.Loaded.BindUnique<FmodAudioSource, &FmodAudioSource::OnEventLoaded>(this);
    Event.Changed.BindUnique<FmodAudioSource, &FmodAudioSource::OnEventChanged>(this);
}

void FmodAudioSource::SetPlayOnStart(bool value)
{
    _playOnStart = value;
}

void FmodAudioSource::SetStartTime(float value)
{
    _startTime = value;
}

void FmodAudioSource::SetOverrideDistance(bool value)
{
    _overrideDistance = value;
    if (CheckForEvent() && Engine::IsPlayMode())
    {
        FmodAudio::GetAudioSystem()->SetEventMaxDistance(EventInstance, value ? _maxDistance : -1.0f);
        FmodAudio::GetAudioSystem()->SetEventMinDistance(EventInstance, value ? _minDistance : -1.0f);
    }
}

void FmodAudioSource::SetMaxDistance(float value)
{
    _maxDistance = value;
    if (CheckForEvent() && Engine::IsPlayMode() && _overrideDistance)
        FmodAudio::GetAudioSystem()->SetEventMaxDistance(EventInstance, value);
}

void FmodAudioSource::SetMinDistance(float value)
{
    _minDistance = value;
    if (CheckForEvent() && Engine::IsPlayMode() && _overrideDistance)
        FmodAudio::GetAudioSystem()->SetEventMinDistance(EventInstance, value);
}

void FmodAudioSource::SetVolumeMultiplier(float value)
{
    _volumeMultiplier = value;
    if (CheckForEvent() && Engine::IsPlayMode())
        FmodAudio::GetAudioSystem()->SetEventVolumeMultiplier(EventInstance, value);
}

void FmodAudioSource::SetPitchMultiplier(float value)
{
    _pitchMultiplier = value;
    if (CheckForEvent() && Engine::IsPlayMode())
        FmodAudio::GetAudioSystem()->SetEventPitchMultiplier(EventInstance, value);
}

void FmodAudioSource::SetEnableBeatEvents(bool value)
{
    _enableBeatEvents = value;
}

void FmodAudioSource::SetEnableMarkerEvents(bool value)
{
    _enableMarkerEvents = value;
}

float FmodAudioSource::GetEventLength()
{
    if (!CheckForEvent())
        return -1.0f;
    return FmodAudio::GetAudioSystem()->GetEventLength(EventInstance);
}

float FmodAudioSource::GetEventPosition()
{
    if (!CheckForEvent())
        return -1.0f;
    return FmodAudio::GetAudioSystem()->GetEventPosition(EventInstance);
}

void FmodAudioSource::SetEventPosition(float position)
{
    if (!CheckForEvent())
        return;
    auto system = FmodAudio::GetAudioSystem();
    float length = system->GetEventLength(EventInstance);
    if (_startTime < length)
        system->SetEventPosition(EventInstance, position);
}

bool FmodAudioSource::CheckForEvent()
{
    if (!EventInstance)
    {
        if (Event)
            Event->WaitForLoaded();
        else
        {
            FMODLOG(Warning, "FmodAudioSource {} has no event set.", GetName());
            return false;
        }
    }
    return true;
}

void FmodAudioSource::Play()
{
    if (!CheckForEvent())
        return;

    if (FmodAudio::GetAudioSystem()->IsEventPaused(EventInstance))
        FmodAudio::GetAudioSystem()->PauseEvent(EventInstance, false);
    if (!IsPlaying())
    {
        auto system = FmodAudio::GetAudioSystem();
        system->RegisterEventCallback(EventInstance, _enableMarkerEvents, _enableBeatEvents);
        float length = system->GetEventLength(EventInstance);
        if (_startTime < length)
            system->SetEventPosition(EventInstance, _startTime);

        system->PlayEvent(EventInstance);
    }
}

void FmodAudioSource::Stop()
{
    if (!CheckForEvent())
        return;

    FmodAudio::GetAudioSystem()->StopEvent(EventInstance, 0);
}

void FmodAudioSource::Pause()
{
    if (!CheckForEvent())
        return;

    FmodAudio::GetAudioSystem()->PauseEvent(EventInstance, true);
}

bool FmodAudioSource::IsPlaying()
{
    if (!CheckForEvent())
        return false;

    bool isPaused = FmodAudio::GetAudioSystem()->IsEventPaused(EventInstance);
    if (isPaused)
        return false;
    return FmodAudio::GetAudioSystem()->IsEventPlaying(EventInstance);
}

bool FmodAudioSource::Is3D()
{
    if (!CheckForEvent())
        return false;

    if (EventInstance && Engine::IsPlayMode())
        return FmodAudio::GetAudioSystem()->IsEvent3D(EventInstance);
    return false;
}

void FmodAudioSource::SetParameter(const String& parameterName, float value)
{
    if (!CheckForEvent())
        return;

    if (EventInstance && Engine::IsPlayMode())
        FmodAudio::GetAudioSystem()->SetEventParameter(EventInstance, parameterName, value);
}

float FmodAudioSource::GetParameter(const String& parameterName)
{
    if (!CheckForEvent())
        return -1.0f;

    if (EventInstance && Engine::IsPlayMode())
        return FmodAudio::GetAudioSystem()->GetEventParameter(EventInstance, parameterName);
    return -1.0f;
}

void FmodAudioSource::OnEnable()
{
    Actor::OnEnable();

#if USE_EDITOR
    GetSceneRendering()->AddViewportIcon(this);
#endif
    
    if (!Engine::IsPlayMode())
        return;

    FmodAudio::Sources.AddUnique(this);
    GetScene()->Ticking.Update.AddTick<FmodAudioSource, &FmodAudioSource::Update>(this);
}

void FmodAudioSource::Update()
{
    // Update the velocity
    const Vector3 pos = GetPosition();
    const float dt = Time::Update.UnscaledDeltaTime.GetTotalSeconds();
    const auto prevVelocity = _velocity;
    _velocity = (pos - _previousPosition) / dt;
    _previousPosition = pos;
}

void FmodAudioSource::OnDisable()
{

#if USE_EDITOR
    GetSceneRendering()->RemoveViewportIcon(this);
#endif
    
    if (Engine::IsPlayMode())
    {
        FmodAudio::Sources.Remove(this);
        GetScene()->Ticking.Update.RemoveTick(this);
        Stop();
    }
    
    Actor::OnDisable();
}

void FmodAudioSource::OnBeginPlay()
{
    Actor::OnBeginPlay();
    
    if (Engine::IsPlayMode())
    {
        if (!CheckForEvent())
            return;

        if (IsActiveInHierarchy() && _playOnStart)
        {
            FmodAudio::Sources.AddUnique(this);
            Play();
        }
    }
}

void FmodAudioSource::OnEndPlay()
{
    if (Engine::IsPlayMode())
    {
        if (EventInstance)
            FmodAudio::GetAudioSystem()->ReleaseEventInstance(EventInstance);
        EventInstance = nullptr;
    }
    Actor::OnEndPlay();
}

#if USE_EDITOR
#include "Engine/Debug/DebugDraw.h"

void FmodAudioSource::OnDebugDrawSelected()
{
    DEBUG_DRAW_WIRE_SPHERE(BoundingSphere(_transform.Translation, _maxDistance), Color::CornflowerBlue, 0, true);
    DEBUG_DRAW_WIRE_SPHERE(BoundingSphere(_transform.Translation, _minDistance), Color::CadetBlue, 0, true);
    
    Actor::OnDebugDrawSelected();
}
#endif

void FmodAudioSource::OnEventLoaded()
{
    if (Engine::IsPlayMode())
    {
        // Create new event
        if (Event)
        {
            auto system = FmodAudio::GetAudioSystem();
            EventInstance = system->CreateEventInstance(Event.GetInstance()->Path, this);
            system->SetEventVolumeMultiplier(EventInstance, _volumeMultiplier);
            system->SetEventPitchMultiplier(EventInstance, _pitchMultiplier);
            system->SetEventMaxDistance(EventInstance, _overrideDistance ? _maxDistance : -1.0f);
            system->SetEventMinDistance(EventInstance, _overrideDistance ? _minDistance : -1.0f);

            // Set initial parameters
            for (int i = 0; i < InitialParameters.Count(); i++)
            {
                auto parameter = InitialParameters[i];
                SetParameter(parameter.Name, parameter.Value);
            }
        }
    }
}

void FmodAudioSource::OnEventChanged()
{
    if (Engine::IsPlayMode())
    {
        // Release old event.
        if (EventInstance)
        {
            FmodAudio::GetAudioSystem()->ReleaseEventInstance(EventInstance);
        }
    }
}

void FmodAudioSource::OnTransformChanged()
{
    Actor::OnTransformChanged();

    _box = BoundingBox(_transform.Translation);
    _sphere = BoundingSphere(_transform.Translation, 0.0f);

    if (Engine::IsPlayMode())
    {

    }
}
