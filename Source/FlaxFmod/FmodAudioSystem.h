#pragma once

#include "fmod_studio.hpp"
#include <fmod.hpp>
#include "Engine/Scripting/Plugins/GamePlugin.h"
#include "Engine/Core/Collections/Dictionary.h"
#include "Engine/Core/Collections/Array.h"

API_CLASS() class FLAXFMOD_API FmodAudioSystem : public GamePlugin
{
    DECLARE_SCRIPTING_TYPE(FmodAudioSystem);

private:
    FMOD::Studio::System* _studioSystem = nullptr;
    FMOD::System* _coreSystem = nullptr;
    Dictionary<StringView, FMOD::Studio::Bank*> _loadedBanks;

    void Update();

    static FMOD_RESULT F_CALL OnSystemEvent(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2, void* userdata);

public:
    void Initialize() override;
    void Deinitialize() override;

    // Master volume
    void SetMasterVolume(float volumeMultiplier);
    float GetMasterVolume();

    // Bank
    void LoadBank(const StringView& bankPath, int loadFlags);
    void LoadBank(const String& bankName);
    void UnloadBank(const StringView& bankPath);
    void UnloadBank(const String& bankName);
    void UnloadAllBanks();
    bool IsBankLoaded(const StringView& bankPath);

    // Event
    void* CreateEventInstance(const StringView& eventPath);
    void* CreateEventInstance(const FMOD_GUID& eventGuid);
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
    float GetEventPosition(void* eventInstance);
    void SetEventPosition(void* eventInstance, float position);

    // System
    void SetDriver(int index);
    int GetDriver();
    void UpdateDrivers();
    void SetGlobalParameter(const StringView& parameterName, float value);
    float GetGlobalParameter(const StringView& parameterName);
};
