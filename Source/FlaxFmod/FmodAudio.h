#pragma once

#include "fmod_studio_common.h"
#include "Assets/FmodBus.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Log.h"
#include "Engine/Audio/AudioDevice.h"
#include "Engine/Content/JsonAssetReference.h"

#define FMODLOG(messageType, format, ...) Log::Logger::Write(LogType::messageType, ::String::Format(String("[Fmod] ").Append(String(format)).GetText(), ##__VA_ARGS__))

class FmodAudioDevice;
class FmodAudioSource;
class FmodAudioListener;
API_CLASS(Static) class FLAXFMOD_API FmodAudio
{
    DECLARE_SCRIPTING_TYPE_NO_SPAWN(FmodAudio);
    friend class FmodAudioSystem;

private:
    static FmodAudioSystem* _audioSystem;
    static int _activeAudioDeviceIndex;

    static void Initialize();
    static void Deinitialize();
    
public:
    /// <summary>
    /// Gets the Fmod audio system game plugin.
    /// </summary>
    API_PROPERTY() static FmodAudioSystem* GetAudioSystem();

    /// <summary>
    /// A list of the enabled fmod listeners.
    /// </summary>
    static Array<FmodAudioListener*> Listeners;

    /// <summary>
    /// The active fmod listener. Based on the last enabled listener.
    /// </summary>
    API_FIELD(ReadOnly) static FmodAudioListener* ActiveListener;

    /// <summary>
    /// A list of the enabled fmod audio sources.
    /// </summary>
    static Array<FmodAudioSource*> Sources;

    /// <summary>
    /// Fired when an audio device is lost.
    /// </summary>
    API_EVENT() static Action AudioDeviceLost;

    /// <summary>
    /// Fired when an audio device list changes.
    /// </summary>
    API_EVENT() static Action AudioDeviceListChanged;

    /// <summary>
    /// Fired when the active audio device changes.
    /// </summary>
    API_EVENT() static Action ActiveAudioDeviceChanged;

    /// <summary>
    /// A list of the connected audio devices.
    /// </summary>
    API_FIELD(ReadOnly) static Array<FmodAudioDevice> AudioDevices;

    /// <summary>
    /// Sets the desired active audio device by index.
    /// </summary>
    API_FUNCTION() static void SetActiveAudioDevice(int index);

    /// <summary>
    /// Gets the active audio device index. Can be used with the AudioDevices list to get the name.
    /// </summary>
    API_FUNCTION() static int GetActiveAudioDevice();

    /// <summary>
    /// Sets the master audio channel volume.
    /// </summary>
    API_FUNCTION() static void SetMasterVolume(float volume);

    /// <summary>
    /// Gets the master audio channel volume.
    /// </summary>
    API_FUNCTION() static float GetMasterVolume();

    /// <summary>
    /// Sets a global FMOD parameter based on name.
    /// </summary>
    API_FUNCTION() static void SetGlobalParameter(const String& name, float value);

    /// <summary>
    /// Gets a global FMOD parameter based on name.
    /// </summary>
    API_FUNCTION() static float GetGlobalParameter(const String& name);

    /// <summary>
    /// Loads a bank based on the bank name. This will resolve the path.
    /// </summary>
    API_FUNCTION() static void LoadBank(const String& bankName);

    /// <summary>
    /// Returns true if the bank is loaded based on the bank name. This will resolve the path.
    /// </summary>
    API_FUNCTION() static bool IsBankLoaded(const String& bankName);

    /// <summary>
    /// Unloads a bank based on the bank name. This will resolve the path.
    /// </summary>
    API_FUNCTION() static void UnloadBank(const String& bankName);

    /// <summary>
    /// Sets a Bus volume multiplier.
    /// </summary>
    API_FUNCTION() static void SetBusVolume(JsonAssetReference<FmodBus> busAsset, float volumeMultiplier);

    /// <summary>
    /// Sets a Bus volume multiplier.
    /// </summary>
    API_FUNCTION() static void SetBusVolume(const String& busPath, float volumeMultiplier);

    /// <summary>
    /// Gets the Bus volume multiplier.
    /// </summary>
    API_FUNCTION() static float GetBusVolume(JsonAssetReference<FmodBus> busAsset);

    /// <summary>
    /// Gets the Bus volume multiplier.
    /// </summary>
    API_FUNCTION() static float GetBusVolume(const String& busPath);

    /// <summary>
    /// Gets whether the Bus is muted.
    /// </summary>
    API_FUNCTION() static bool GetBusMute(JsonAssetReference<FmodBus> busAsset);

    /// <summary>
    /// Gets whether the Bus is muted.
    /// </summary>
    API_FUNCTION() static bool GetBusMute(const String& busPath);

    /// <summary>
    /// Sets whether the Bus is muted.
    /// </summary>
    API_FUNCTION() static void SetBusMute(JsonAssetReference<FmodBus> busAsset, bool value);

    /// <summary>
    /// Sets whether the Bus is muted.
    /// </summary>
    API_FUNCTION() static void SetBusMute(const String& busPath, bool value);
};
