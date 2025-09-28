#pragma once

#include "FmodAudioSettings.h"
#include "fmod_studio.hpp"
#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Core/Collections/Array.h"

class FmodAudioSource;

API_CLASS() class FLAXFMOD_API FmodAudioSystem : public GamePlugin
{
    DECLARE_SCRIPTING_TYPE(FmodAudioSystem);

private:
    FMOD::Studio::System* _studioSystem = nullptr;
    FMOD::System* _coreSystem = nullptr;
    FmodAudioSettings* _settings;
    static Dictionary<FMOD::Studio::EventInstance*, FmodAudioSource*> EventMap;
    Dictionary<StringView, FMOD::Studio::Bank*> _loadedBanks;
    Array<uint32> _loadedPlugins;

    void Update();

    static FMOD_RESULT F_CALL OnSystemCallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2, void* userdata);
    static FMOD_RESULT F_CALL OnEventInstanceCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters);

public:
    void Initialize() override;
    void Deinitialize() override;

    // Master
    void SetMasterVolume(float volumeMultiplier);
    float GetMasterVolume();
    void SetMasterPaused(bool paused);
    bool IsMasterPaused();
    void SetMasterPitch(float value);
    float GetMasterPitch();

    // Bank
    void LoadBank(const StringView& bankPath, int loadFlags, bool loadSampleData);
    void LoadBank(const String& bankName, bool loadSampleData);
    void UnloadBank(const StringView& bankPath);
    void UnloadBank(const String& bankName);
    void UnloadAllBanks();
    bool IsBankLoaded(const StringView& bankPath);
    bool CheckBankLoaded(const String& bankName);

    // Event
    void* CreateEventInstance(const StringView& eventPath, FmodAudioSource* source);
    void* CreateEventInstance(const FMOD_GUID& eventGuid, FmodAudioSource* source);
    void ReleaseEventInstance(void* eventInstance);
    void PlayEvent(void* eventInstance);
    bool IsEventPlaying(void* eventInstance);
    void StopEvent(void* eventInstance, int stopMode);
    void PauseEvent(void* eventInstance, bool pause);
    bool IsEventPaused(void* eventInstance);
    bool IsEvent3D(void* eventInstance);
    void SetEventVolumeMultiplier(void* eventInstance, float volumeScale);
    void SetEventPitchMultiplier(void* eventInstance, float pitch);
    float GetEventMinDistance(void* eventInstance);
    float GetEventMaxDistance(void* eventInstance);
    void SetEventMaxDistance(void* eventInstance, float maxDistance);
    void SetEventMinDistance(void* eventInstance, float minDistance);
    void SetEventParameter(void* eventInstance, const StringView& parameterName, float value);
    float GetEventParameter(void* eventInstance, const StringView& parameterName);
    float GetEventLength(void* eventInstance);
    float GetEventLength(const String& eventPath);
    float GetEventPosition(void* eventInstance);
    void SetEventPosition(void* eventInstance, float position);
    void RegisterEventCallback(void* eventInstance, bool marker, bool beat);

    // System
    void SetDriver(int index);
    int GetDriver();
    void UpdateDrivers();
    void SetGlobalParameter(const StringView& parameterName, float value);
    float GetGlobalParameter(const StringView& parameterName);

    // Bus
    void SetBusMute(const String& busPath, bool mute);
    bool GetBusMute(const String& busPath);
    void SetBusVolumeMultiplier(const String& busPath, float volumeScale);
    float GetBusVolumeMultiplier(const String& busPath);
    void SetBusPaused(const String& busPath, bool paused);
    bool IsBusPaused(const String& busPath);

    // VCA
    void SetVCAVolumeMultiplier(const String& vcaPath, float volumeScale);
    float GetVCAVolumeMultiplier(const String& vcaPath);
};
