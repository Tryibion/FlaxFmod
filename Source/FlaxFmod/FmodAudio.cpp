#include "FmodAudio.h"

#include "FmodAudioSettings.h"
#include "FmodAudioSystem.h"
#include "Engine/Scripting/Plugins/PluginManager.h"
#include "Actors/FmodAudioListener.h"
#include "Actors/FmodAudioSource.h"
#include "Engine/Level/Level.h"
#include "Types/FmodAudioDevice.h"

FmodAudioSystem* FmodAudio::_audioSystem = nullptr;
Array<FmodAudioListener*> FmodAudio::Listeners;
FmodAudioListener* FmodAudio::ActiveListener;
Array<FmodAudioSource*> FmodAudio::Sources;
Delegate<> FmodAudio::AudioDeviceListChanged;
Delegate<> FmodAudio::AudioDeviceLost;
Delegate<> FmodAudio::ActiveAudioDeviceChanged;
Array<FmodAudioDevice> FmodAudio::AudioDevices;
int FmodAudio::_activeAudioDeviceIndex;

void FmodAudio::Initialize()
{
    _audioSystem = PluginManager::GetPlugin<FmodAudioSystem>();
    _activeAudioDeviceIndex = -1;
}

void FmodAudio::Deinitialize()
{
    _activeAudioDeviceIndex = -1;
    _audioSystem = nullptr;
}

FmodAudioSystem* FmodAudio::GetAudioSystem()
{
    if (_audioSystem)
        return _audioSystem;
    return PluginManager::GetPlugin<FmodAudioSystem>();
}

void FmodAudio::SetActiveAudioDevice(int index)
{
    if (!_audioSystem)
        return;
    // Ignore first call to only set index.
    if (_activeAudioDeviceIndex != -1)
    {
        _audioSystem->SetDriver(index);
        ActiveAudioDeviceChanged();
    }
    _activeAudioDeviceIndex = index;
}

int FmodAudio::GetActiveAudioDevice()
{
    return _activeAudioDeviceIndex;
}

void FmodAudio::PlayEventAtLocation(const JsonAssetReference<FmodEvent>& fmodEvent, const Vector3& location)
{
    if (!_audioSystem)
        return;

    // TODO: object pooling for better memory efficiency
    FmodAudioSource* source = New<FmodAudioSource>();
    Level::SpawnActor(source);
    source->Event = fmodEvent;
    source->SetPosition(location);
    source->Play();
    source->DeleteObject(source->GetEventLength(), true);
}

void FmodAudio::SetMasterVolume(float volume)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetMasterVolume(volume);
}

float FmodAudio::GetMasterVolume()
{
    if (!_audioSystem)
        return -1.0f;
    return _audioSystem->GetMasterVolume();
}

void FmodAudio::SetMasterPaused(bool paused)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetMasterPaused(paused);
}

bool FmodAudio::GetMasterPaused()
{
    if (!_audioSystem)
        return false;
    return _audioSystem->IsMasterPaused();
}

void FmodAudio::SetMasterPitch(float pitch)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetMasterPitch(pitch);
}

float FmodAudio::GetMasterPitch()
{
    if (!_audioSystem)
        return -1.0f;
    return _audioSystem->GetMasterPitch();
}

void FmodAudio::SetGlobalParameter(const String& name, float value)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetGlobalParameter(name, value);
}

float FmodAudio::GetGlobalParameter(const String& name)
{
    if (!_audioSystem)
        return -1.0f;
    return _audioSystem->GetGlobalParameter(name);
}

void FmodAudio::LoadBank(const String& bankName)
{
    if (!_audioSystem)
        return;
   _audioSystem->LoadBank(bankName);
}

bool FmodAudio::IsBankLoaded(const String& bankName)
{
    if (!_audioSystem)
        return false;
    return _audioSystem->CheckBankLoaded(bankName);
}

void FmodAudio::UnloadBank(const String& bankName)
{
    if (!_audioSystem)
        return;
    _audioSystem->UnloadBank(bankName);
}

void FmodAudio::SetBusVolume(JsonAssetReference<FmodBus> busAsset, float volumeMultiplier)
{
    if (!_audioSystem)
        return;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return;
    _audioSystem->SetBusVolumeMultiplier(bus->Path, volumeMultiplier);
}

void FmodAudio::SetBusVolume(const String& busPath, float volumeMultiplier)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetBusVolumeMultiplier(busPath, volumeMultiplier);
}

float FmodAudio::GetBusVolume(JsonAssetReference<FmodBus> busAsset)
{
    if (!_audioSystem)
        return -1.0f;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return -1.0f;
    return _audioSystem->GetBusVolumeMultiplier(bus->Path);
}

float FmodAudio::GetBusVolume(const String& busPath)
{
    if (!_audioSystem)
        return -1.0f;
    return _audioSystem->GetBusVolumeMultiplier(busPath);
}

bool FmodAudio::GetBusMute(JsonAssetReference<FmodBus> busAsset)
{
    if (!_audioSystem)
        return false;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return false;
    return _audioSystem->GetBusMute(bus->Path);
}

bool FmodAudio::GetBusMute(const String& busPath)
{
    if (!_audioSystem)
        return false;
    return _audioSystem->GetBusMute(busPath);
}

void FmodAudio::SetBusMute(JsonAssetReference<FmodBus> busAsset, bool value)
{
    if (!_audioSystem)
        return;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return;
    _audioSystem->SetBusMute(bus->Path, value);
}

void FmodAudio::SetBusMute(const String& busPath, bool value)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetBusMute(busPath, value);
}

void FmodAudio::SetBusPaused(JsonAssetReference<FmodBus> busAsset, bool value)
{
    if (!_audioSystem)
        return;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return;
    _audioSystem->SetBusPaused(bus->Path, value);
}

void FmodAudio::SetBusPaused(const String& busPath, bool value)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetBusPaused(busPath, value);
}

bool FmodAudio::IsBusPaused(JsonAssetReference<FmodBus> busAsset)
{
    if (!_audioSystem)
        return false;
    auto* bus = busAsset->GetInstance<FmodBus>();
    if (!bus)
        return false;
    return _audioSystem->IsBusPaused(bus->Path);
}

bool FmodAudio::IsBusPaused(const String& busPath)
{
    if (!_audioSystem)
        return false;
    return _audioSystem->IsBusPaused(busPath);
}

void FmodAudio::SetVCAVolume(JsonAssetReference<FmodVca> vcaAsset, float volumeScale)
{
    if (!_audioSystem)
        return;
    auto* vca = vcaAsset->GetInstance<FmodVca>();
    if (!vca)
        return;
    _audioSystem->SetVCAVolumeMultiplier(vca->Path, volumeScale);
}

void FmodAudio::SetVCAVolume(const String& vcaPath, float volumeScale)
{
    if (!_audioSystem)
        return;
    _audioSystem->SetVCAVolumeMultiplier(vcaPath, volumeScale);
}

float FmodAudio::GetVCAVolume(JsonAssetReference<FmodVca> vcaAsset)
{
    if (!_audioSystem)
        return -1.0f;
    auto* vca = vcaAsset->GetInstance<FmodVca>();
    if (!vca)
        return -1.0f;
    return _audioSystem->GetVCAVolumeMultiplier(vca->Path);
}

float FmodAudio::GetVCAVolume(const String& vcaPath)
{
    if (!_audioSystem)
        return -1.0f;
    return _audioSystem->GetVCAVolumeMultiplier(vcaPath);
}