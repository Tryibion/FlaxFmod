#pragma once

#include "Engine/Content/JsonAssetReference.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Level/Actor.h"
#include "FlaxFmod/Assets/FmodEvent.h"
#include "FlaxFmod/Types/FmodParameter.h"

API_CLASS(Attributes="ActorContextMenu(\"New/Audio/Fmod Audio Source\"), ActorToolbox(\"Other\")")
class FLAXFMOD_API FmodAudioSource : public Actor
{
API_AUTO_SERIALIZATION();
DECLARE_SCENE_OBJECT(FmodAudioSource);

private:
    Vector3 _previousPosition;
    Vector3 _velocity;
    bool _playOnStart = false;
    bool _overrideDistance = false;
    float _volumeMultiplier = 1.0f;
    float _pitchMultiplier = 1.0f;
    float _maxDistance = 1000.0f;
    float _minDistance = 0.1f;
    float _startTime = 0.0f;
    bool _enableBeatEvents = false;
    bool _enableMarkerEvents = true;
    bool _allowFadeout = false;
    
public:

    /// <summary>
    /// Fired when the event is starting to play.
    /// </summary>
    API_EVENT() Action EventStarting;

    /// <summary>
    /// Fired when the event is started.
    /// </summary>
    API_EVENT() Action EventStarted;

    /// <summary>
    /// Fired when a sub event is started.
    /// </summary>
    API_EVENT() Action SubEventStarted;

    /// <summary>
    /// Fired when the event is stopped.
    /// </summary>
    API_EVENT() Action EventStopped;

    /// <summary>
    /// Fired when the event is restarted by calling play again.
    /// </summary>
    API_EVENT() Action EventRestarted;

    /// <summary>
    /// Fired if timeline beat is enabled on the beat.
    /// Parameters are Beat, Bar, tempo, position
    /// </summary>
    API_EVENT() Delegate<int, int, float, int> TimelineBeat;

    /// <summary>
    /// Fired if timeline beat is enabled on the sub timeline beat.
    /// Parameters are Beat, Bar, tempo, position
    /// </summary>
    API_EVENT() Delegate<int, int, float, int> SubTimelineBeat;

    /// <summary>
    /// Fired if timeline marker is enabled when a timeline marker occurs.
    /// Gives the marker name and position.
    /// </summary>
    API_EVENT() Delegate<String, int> TimelineMarker;
    
    /// <summary>
    /// The fmod event asset.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(10)")
    JsonAssetReference<FmodEvent> Event;

    /// <summary>
    /// The fmod event instance used for connection with the fmod backend.
    /// </summary>
    void* EventInstance = nullptr;

    /// <summary>
    /// The start time for event.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), Limit(0), EditorOrder(21)")
    FORCE_INLINE float GetStartTime() const
    {
        return _startTime;
    }

    /// <summary>
    /// The start time for event.
    /// </summary>
    API_PROPERTY()
    void SetStartTime(float value);

    /// <summary>
    /// Whether to play the event on start and after it is loaded.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(20)")
    FORCE_INLINE bool GetPlayOnStart() const
    {
        return _playOnStart;
    }

    /// <summary>
    /// Whether to play the event on start and after it is loaded.
    /// </summary>
    API_PROPERTY()
    void SetPlayOnStart(bool value);

    /// <summary>
    /// Whether to override the distance properties for the event with the min and max distance set.
    /// Note: To use this, do not set the override distance in fmod studio.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(30)")
    FORCE_INLINE bool GetOverrideDistance() const
    {
        return _overrideDistance;
    }

    /// <summary>
    /// Whether to override the distance properties for the event with the Min and Max distance set.
    /// Note: To use this, do not set the override distance in fmod studio.
    /// </summary>
    API_PROPERTY()
    void SetOverrideDistance(bool value);

    /// <summary>
    /// The maximum distance for the event. Only used when OverrideDistance is true.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(31)")
    FORCE_INLINE float GetMaxDistance() const
    {
        return _maxDistance;
    }

    /// <summary>
    /// The maximum distance for the event. Only used when OverrideDistance is true.
    /// </summary>
    API_PROPERTY()
    void SetMaxDistance(float value);

    /// <summary>
    /// The minimum distance for the event. Only used when OverrideDistance is true.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(32)")
    FORCE_INLINE float GetMinDistance() const
    {
        return _minDistance;
    }

    /// <summary>
    /// The minimum distance for the event. Only used when OverrideDistance is true.
    /// </summary>
    API_PROPERTY()
    void SetMinDistance(float value);

    /// <summary>
    /// The volume multiplier for the event to use.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), Limit(0), EditorOrder(40)")
    FORCE_INLINE float GetVolumeMultiplier() const
    {
        return _volumeMultiplier;
    }

    /// <summary>
    /// The volume multiplier for the event to use.
    /// </summary>
    API_PROPERTY()
    void SetVolumeMultiplier(float value);

    /// <summary>
    /// The pitch multiplier for the event to use.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), Limit(0), EditorOrder(41)")
    FORCE_INLINE float GetPitchMultiplier() const
    {
        return _pitchMultiplier;
    }

    /// <summary>
    /// The pitch multiplier for the event to use.
    /// </summary>
    API_PROPERTY()
    void SetPitchMultiplier(float value);

    /// <summary>
    /// Whether to allow fadeout when stopped.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), Limit(0), EditorOrder(42)")
    FORCE_INLINE bool GetAllowFadeout() const
    {
        return _allowFadeout;
    }

    /// <summary>
    /// Whether to allow fadeout when stopped.
    /// </summary>
    API_PROPERTY()
    void SetAllowFadeout(bool value);

    /// <summary>
    /// Whether to enable the beat event for this audio source.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(50)")
    FORCE_INLINE bool GetEnableBeatEvents() const
    {
        return _enableBeatEvents;
    }

    /// <summary>
    /// Whether to enable the beat event for this audio source.
    /// </summary>
    API_PROPERTY()
    void SetEnableBeatEvents(bool value);

    /// <summary>
    /// Whether to enable the marker event for this audio source.
    /// </summary>
    API_PROPERTY(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(51)")
    FORCE_INLINE bool GetEnableMarkerEvents() const
    {
        return _enableMarkerEvents;
    }

    /// <summary>
    /// Whether to enable the marker event for this audio source.
    /// </summary>
    API_PROPERTY()
    void SetEnableMarkerEvents(bool value);

    /// <summary>
    /// The initial parameters to set on the event.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Fmod Audio Source\"), EditorOrder(60)")
    Array<FmodParameter> InitialParameters;

    /// <summary>
    /// Gets the velocity of the audio source.
    /// </summary>
     API_FUNCTION() FORCE_INLINE Vector3 GetVelocity() const
    {
        return _velocity;
    }

    /// <summary>
    /// Gets the event length.
    /// </summary>
    API_FUNCTION() float GetEventLength();

    /// <summary>
    /// Gets the current event position.
    /// </summary>
    API_FUNCTION() float GetEventPosition();

    /// <summary>
    /// Sets the event position.
    /// </summary>
    API_FUNCTION() void SetEventPosition(float position);

    /// <summary>
    /// Plays the event.
    /// </summary>
    API_FUNCTION() void Play();

    /// <summary>
    /// Stops the event.
    /// </summary>
    API_FUNCTION() void Stop();

    /// <summary>
    /// Pauses the event.
    /// </summary>
    API_FUNCTION() void Pause();

    /// <summary>
    /// Gets if the event is playing.
    /// </summary>
    API_FUNCTION() bool IsPlaying();

    /// <summary>
    /// Gets if the event playing is 3D or not.
    /// </summary>
    API_FUNCTION() bool Is3D();

    /// <summary>
    /// Sets a event parameter based on its name.
    /// </summary>
    API_FUNCTION() void SetParameter(const String& parameterName, float value);

    /// <summary>
    /// Gets a event parameter based on its name.
    /// </summary>
    API_FUNCTION() float GetParameter(const String& parameterName);

private:

    // [Actor]
    void OnEnable() override;
    void Update();
    void OnDisable() override;
    void OnBeginPlay() override;
    void OnEndPlay() override;

#if USE_EDITOR
    void OnDebugDrawSelected() override;
#endif

    void OnEventLoaded();
    void OnEventChanged();
    bool CheckForEvent();

    void OnTransformChanged() override;
};
