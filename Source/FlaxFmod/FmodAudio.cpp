#include "FmodAudio.h"

#include "FmodAudioSettings.h"
#include "FmodAudioSystem.h"
#include "Engine/Scripting/Plugins/PluginManager.h"
#include "Actors/FmodAudioListener.h"
#include "Actors/FmodAudioSource.h"
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

void FmodAudio::UnloadBank(const String& bankName)
{
    if (!_audioSystem)
        return;
    _audioSystem->UnloadBank(bankName);
}
