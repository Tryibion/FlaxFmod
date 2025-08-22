#include "FmodAudioSystem.h"

#include "FmodAudio.h"
#include "Types/FmodAudioDevice.h"
#include "FmodAudioSettings.h"
#include "Engine/Platform/Types.h"
#include "fmod_errors.h"
#include "Actors/FmodAudioListener.h"
#include "Actors/FmodAudioSource.h"
#include "Engine/Content/Content.h"
#include "Engine/Engine/Engine.h"
#include "Engine/Engine/Globals.h"
#include "Engine/Scripting/Scripting.h"

#if PLATFORM_WINDOWS
#include "Engine/Platform/Windows/WindowsFileSystem.h"
#elif PLATFORM_LINUX
#include "Engine/Platform/Linux/LinuxFileSystem.h"
#elif PLATFORM_MACOS
#include "Engine/Platform/MacOS/MacOSFileSystem.h"
#elif PLATFORM_ANDROID
#include "Engine/Platform/Android/AndroidFileSystem.h"
#elif PLATFORM_IOS
#include "Engine/Platform/IOS/IOSFileSystem.h"
#elif PLATFORM_SWITCH
#include "Engine/Platform/Switch/SwitchFileSystem.h"
#elif PLATFORM_PS4
#endif

FmodAudioSystem::FmodAudioSystem(const SpawnParams& params)
    : GamePlugin(params)
{
}

void FmodAudioSystem::Update()
{
    if (_studioSystem)
    {
        // TODO: support multiple listeners.
        // Update active listener
        FmodAudioListener* activeListener = FmodAudio::ActiveListener;
        if (activeListener)
        {
            FMOD_3D_ATTRIBUTES listenerAttributes;
            Vector3 listenerPosition = activeListener->GetPosition();
            Vector3 listenerVelocity = activeListener->GetVelocity();
            Vector3 listenerForward = activeListener->GetDirection();
            Vector3 listenerUp = activeListener->GetTransform().GetUp();
            listenerAttributes.position = { static_cast<float>(listenerPosition.X), static_cast<float>(listenerPosition.Y), static_cast<float>(listenerPosition.Z) };
            listenerAttributes.velocity = { static_cast<float>(listenerVelocity.X), static_cast<float>(listenerVelocity.Y), static_cast<float>(listenerVelocity.Z) };
            listenerAttributes.forward = { static_cast<float>(listenerForward.X), static_cast<float>(listenerForward.Y), static_cast<float>(listenerForward.Z) };
            listenerAttributes.up = { static_cast<float>(listenerUp.X), static_cast<float>(listenerUp.Y), static_cast<float>(listenerUp.Z) };

            _studioSystem->setListenerAttributes(0, &listenerAttributes);
        }

        // Update sources/events
        auto sources = FmodAudio::Sources;
        for (int i = 0; i < sources.Count(); ++i)
        {
            const auto source = sources[i];
            const auto eventInstance = source->EventInstance;
            if (eventInstance == nullptr)
                continue;

            auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
            if (!instance->isValid())
                continue;

            if (!IsEvent3D(instance))
                continue;

            FMOD_3D_ATTRIBUTES sourceAttributes;
            Vector3 sourcePosition = source->GetPosition();
            Vector3 sourceVelocity = source->GetVelocity();
            Vector3 sourceForward = source->GetDirection();
            Vector3 sourceUp = source->GetTransform().GetUp();
            sourceAttributes.position = { static_cast<float>(sourcePosition.X), static_cast<float>(sourcePosition.Y), static_cast<float>(sourcePosition.Z) };
            sourceAttributes.velocity = { static_cast<float>(sourceVelocity.X), static_cast<float>(sourceVelocity.Y), static_cast<float>(sourceVelocity.Z) };
            sourceAttributes.forward = { static_cast<float>(sourceForward.X), static_cast<float>(sourceForward.Y), static_cast<float>(sourceForward.Z) };
            sourceAttributes.up = { static_cast<float>(sourceUp.X), static_cast<float>(sourceUp.Y), static_cast<float>(sourceUp.Z) };
            instance->set3DAttributes(&sourceAttributes);
        }

        const auto result = _studioSystem->update();
        if (result != FMOD_OK)
            FMODLOG(Warning, "Failed to update Fmod studio system. Error: {}", *FMOD_ErrorString(result));
    }
}

FMOD_RESULT FmodAudioSystem::OnSystemEvent(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2, void* userdata)
{
    auto* audioSystem = static_cast<FmodAudioSystem*>(userdata);
    if (!audioSystem)
    {
        FMODLOG(Warning, "Fmod audio system is not set to userdata in system callback.");
        return FMOD_OK;
    }

    if (type == FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED)
    {
        audioSystem->UpdateDrivers();
        FmodAudio::AudioDeviceListChanged();
    }
    else if (type == FMOD_SYSTEM_CALLBACK_DEVICELOST)
    {
        audioSystem->UpdateDrivers();
        FmodAudio::AudioDeviceLost();
    }

    return FMOD_OK;
}

void FmodAudioSystem::Initialize()
{
    auto result = FMOD::Studio::System::create(&_studioSystem);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to create Fmod studio system. Error: {}", *FMOD_ErrorString(result));
        return;
    }

    auto* settings = FmodAudioSettings::Get();

    // TODO: make parameters into settings
    result = _studioSystem->initialize(settings->MaxChannels, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to initialize Fmod studio system. Error: {}", *FMOD_ErrorString(result));
        return;
    }

    result = _studioSystem->getCoreSystem(&_coreSystem);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get Fmod core system. Error: {}", *FMOD_ErrorString(result));
        return;
    }

    // TODO: expose doppler scale and rolloff scale to settings
    // Distance factor is 0.01f for centimeters. 1 = 1 meter.
    _coreSystem->set3DSettings(1, 0.01f, 1);

    _coreSystem->setUserData(this); // To use in events
    _coreSystem->setCallback(&OnSystemEvent, FMOD_SYSTEM_CALLBACK_DEVICELISTCHANGED | FMOD_SYSTEM_CALLBACK_DEVICELOST);

    FmodAudio::Initialize();

    FMODLOG(Info, "Fmod studio system created and initialized.");

    UpdateDrivers();
    FmodAudio::SetActiveAudioDevice(GetDriver());
    auto activeAudioDevice = FmodAudio::AudioDevices[FmodAudio::GetActiveAudioDevice()];
    FMODLOG(Info, "Active audio device: {}.", activeAudioDevice.Name);

    Scripting::Update.Bind<FmodAudioSystem, &FmodAudioSystem::Update>(this);

    // Always load master banks
#if USE_EDITOR
    // Todo: make this work for other platforms once supported.
    auto relativeBankPath = settings->EditorStorageRelativeFolderPath + TEXT("/") + TEXT("Build") + TEXT("/") + TEXT("Desktop");
#else
    auto relativeBankPath = settings->BuiltProjectBankRelativeFolderPath;
#endif

    String masterBankName = settings->MasterBankName;
    StringView masterBankFileName = masterBankName + TEXT(".bank");
    StringView masterBankStringsFileName = masterBankName + TEXT(".strings.bank");

    auto bankFolder = Globals::ProjectFolder + TEXT("/") + relativeBankPath;
    FileSystem::NormalizePath(bankFolder);
    Array<String> bankFiles;
    FileSystem::DirectoryGetFiles(bankFiles, bankFolder, TEXT("*.bank"));

    // Load master bank first.
    for (const auto& bankFile : bankFiles)
    {
        if (StringUtils::GetFileName(bankFile).Compare(masterBankFileName) == 0 || StringUtils::GetFileName(bankFile).Compare(masterBankStringsFileName) == 0)
            LoadBank(bankFile, FMOD_STUDIO_LOAD_BANK_NORMAL);
    }

    // Load the rest of the banks if requested.
    if (settings->LoadAllBanks)
    {
        // Load all other banks
        for (const auto& bankFile : bankFiles)
        {
            if (StringUtils::GetFileName(bankFile).Compare(masterBankFileName) == 0 || StringUtils::GetFileName(bankFile).Compare(masterBankStringsFileName) == 0)
                continue;
            LoadBank(bankFile, FMOD_STUDIO_LOAD_BANK_NORMAL);
        }
    }
    // Load specifically defined banks
    else
    {
        for (auto preloadBankName : settings->BanksToPreload)
        {
            StringView preloadFileName;
            if (!preloadBankName.EndsWith(TEXT(".bank")))
                preloadFileName = preloadBankName + TEXT(".bank");
            else
                preloadFileName = preloadBankName;
            
            // Skip master bank if defined
            if (StringUtils::GetFileName(preloadFileName).Compare(masterBankFileName) == 0 || StringUtils::GetFileName(preloadFileName).Compare(masterBankStringsFileName) == 0)
                continue;
            
            for (const auto& bankFile : bankFiles)
            {
                if (StringUtils::GetFileName(bankFile).Compare(preloadFileName) == 0)
                {
                    LoadBank(bankFile, FMOD_STUDIO_LOAD_BANK_NORMAL);
                    break;
                }
            }
        }
    }
}

void FmodAudioSystem::Deinitialize()
{
    FmodAudio::Deinitialize();

    UnloadAllBanks();

    Scripting::Update.Unbind<FmodAudioSystem, &FmodAudioSystem::Update>(this);

    if (_studioSystem)
    {
        auto result = _studioSystem->release();
        if (result != FMOD_OK)
            FMODLOG(Warning, "Failed to release Fmod studio system. Error: {}", String(FMOD_ErrorString(result)));
        _studioSystem = nullptr;
    }
}

void FmodAudioSystem::SetMasterVolume(float volumeMultiplier)
{
    FMOD::ChannelGroup* masterChannelGroup = nullptr;
    auto result = _coreSystem->getMasterChannelGroup(&masterChannelGroup);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get master channel group to set master volume, Error: {}",
                String(FMOD_ErrorString(result)));
        return;
    }
    result = masterChannelGroup->setVolume(volumeMultiplier);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to set master volume level, Error: {}", String(FMOD_ErrorString(result)));
        return;
    }
    FMODLOG(Info, "Master volume set to {}.", volumeMultiplier);
}

float FmodAudioSystem::GetMasterVolume()
{
    FMOD::ChannelGroup* masterChannelGroup = nullptr;
    auto result = _coreSystem->getMasterChannelGroup(&masterChannelGroup);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get master channel group to get master volume, Error: {}",
                String(FMOD_ErrorString(result)));
        return -1.0f;
    }

    float volume = -1.0f;
    result = masterChannelGroup->getVolume(&volume);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get master volume level, Error: {}", String(FMOD_ErrorString(result)));
        return -1.0f;
    }
    return volume;
}

void FmodAudioSystem::LoadBank(const StringView& bankPath, int loadFlags)
{
    if (IsBankLoaded(bankPath))
        return;

    FMOD::Studio::Bank* bank = nullptr;
    auto result = _studioSystem->loadBankFile(bankPath.ToStringAnsi().GetText(), static_cast<FMOD_STUDIO_LOAD_BANK_FLAGS>(loadFlags), &bank);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to load bank at {}, Error: {}", bankPath, String(FMOD_ErrorString(result)));
        return;
    }
    _loadedBanks.Add(bankPath, bank);
    FMODLOG(Info, "Bank {} loaded.", bankPath);
}

void FmodAudioSystem::LoadBank(const String& bankName)
{
    // Search for bank file in paths.
    auto* settings = FmodAudioSettings::Get();

#if USE_EDITOR
    // Todo: make this work for other platforms once supported.
    auto relativeBankPath = settings->EditorStorageRelativeFolderPath + TEXT("/") + TEXT("Build") + TEXT("/") + TEXT("Desktop");
#else
    auto relativeBankPath = settings->BuiltProjectBankRelativeFolderPath;
#endif

    String bankFileName;
    if (!bankName.EndsWith(TEXT(".bank")))
        bankFileName = bankName + TEXT(".bank");
    else
        bankFileName = bankName;
    
    auto bankFolder = Globals::ProjectFolder + TEXT("/") + relativeBankPath;
    FileSystem::NormalizePath(bankFolder);
    Array<String> bankFiles;
    FileSystem::DirectoryGetFiles(bankFiles, bankFolder, bankFileName.Get());

    if (bankFiles.Count() == 0)
    {
        FMODLOG(Warning, "Can not load bank. Bank {} not found.", bankName);
        return;
    }

    auto bankPath = bankFiles[0];
    
    if (IsBankLoaded(bankPath))
        return;

    FMOD::Studio::Bank* bank = nullptr;
    auto result = _studioSystem->loadBankFile(bankPath.ToStringAnsi().GetText(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to load bank at {}, Error: {}", bankPath, String(FMOD_ErrorString(result)));
        return;
    }
    _loadedBanks.Add(bankPath, bank);
    FMODLOG(Info, "Bank {} loaded.", bankPath);
}

void FmodAudioSystem::UnloadBank(const StringView& bankPath)
{
    FMOD::Studio::Bank* bank = nullptr;
    if (_loadedBanks.TryGet(bankPath, bank))
    {
        bank->unload();
        _loadedBanks.Remove(bankPath);
        FMODLOG(Info, "Bank {} unloaded.", bankPath);
    }
}

void FmodAudioSystem::UnloadBank(const String& bankName)
{
    String bankFileName;
    if (!bankName.EndsWith(TEXT(".bank")))
        bankFileName = bankName + TEXT(".bank");
    else
        bankFileName = bankName;
    
    for (auto& bank : _loadedBanks)
    {
        if (bank.Key.EndsWith(bankFileName))
        {
            bank.Value->unload();
            FMODLOG(Info, "Bank {} unloaded.", bank.Key);
            _loadedBanks.Remove(bank.Key);
            break;
        }
    }
}

void FmodAudioSystem::UnloadAllBanks()
{
    for (auto& bank : _loadedBanks)
    {
        bank.Value->unload();
    }
    _loadedBanks.Clear();
    FMODLOG(Info, "All banks unloaded.");
}

bool FmodAudioSystem::IsBankLoaded(const StringView& bankPath)
{
    FMOD::Studio::Bank* fmodBank;
    if (_loadedBanks.TryGet(bankPath, fmodBank))
    {
        FMOD_STUDIO_LOADING_STATE state;
        FMOD_RESULT result = fmodBank->getLoadingState(&state);
        if (result == FMOD_OK && state == FMOD_STUDIO_LOADING_STATE_LOADED)
        {
            return true;
        }
    }

    return false;
}

bool FmodAudioSystem::CheckBankLoaded(const String& bankName)
{
    String bankFileName;
    if (!bankName.EndsWith(TEXT(".bank")))
        bankFileName = bankName + TEXT(".bank");
    else
        bankFileName = bankName;
    
    for (auto& bank : _loadedBanks)
    {
        if (bank.Key.EndsWith(bankFileName))
        {
            return IsBankLoaded(bank.Key);
        }
    }

    return false;
}

void* FmodAudioSystem::CreateEventInstance(const StringView& eventPath)
{
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    FMOD_RESULT result = _studioSystem->getEvent(eventPath.ToStringAnsi().GetText(), &eventDescription);

    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get event description at {}, Error: {}", eventPath,
                String(FMOD_ErrorString(result)));
        return nullptr;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    result = eventDescription->createInstance(&eventInstance);

    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to create event instance at {}, Error: {}", eventPath,
                String(FMOD_ErrorString(result)));
        return nullptr;
    }

    FMODLOG(Info, "Event {} created.", eventPath);
    return eventInstance;
}

void* FmodAudioSystem::CreateEventInstance(const FMOD_GUID& eventGuid)
{
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    FMOD_RESULT result = _studioSystem->getEventByID(&eventGuid, &eventDescription);

    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get event description, Error: {}", String(FMOD_ErrorString(result)));
        return nullptr;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    result = eventDescription->createInstance(&eventInstance);

    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to create event instance, Error: {}", String(FMOD_ErrorString(result)));
        return nullptr;
    }

    return eventInstance;
}

void FmodAudioSystem::ReleaseEventInstance(void* eventInstance)
{
    if (!eventInstance)
        return;

    FMOD::Studio::EventInstance* instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);

    // Stop event if playing.
    auto result = instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to stop event instance. Error: {}", String(FMOD_ErrorString(result)));
        return;
    }

    result = instance->release();
    if (result != FMOD_OK)
        FMODLOG(Warning, "Failed to release event instance. Error: {}", String(FMOD_ErrorString(result)));
    FMODLOG(Info, "Event released.");
    eventInstance = nullptr;
}

void FmodAudioSystem::PlayEvent(void* eventInstance)
{
    if (!eventInstance)
        return;

    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->start();
}

bool FmodAudioSystem::IsEventPlaying(void* eventInstance)
{
    FMOD_STUDIO_PLAYBACK_STATE state;
    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getPlaybackState(&state);

    if (state == FMOD_STUDIO_PLAYBACK_PLAYING)
        return true;

    return false;
}

void FmodAudioSystem::StopEvent(void* eventInstance, int stopMode)
{
    if (!eventInstance)
        return;

    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->stop(static_cast<FMOD_STUDIO_STOP_MODE>(stopMode));
}

void FmodAudioSystem::PauseEvent(void* eventInstance, bool pause)
{
    if (!eventInstance)
        return;

    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->setPaused(pause);
}

bool FmodAudioSystem::IsEventPaused(void* eventInstance)
{
    if (!eventInstance)
        return false;

    bool result;
    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getPaused(&result);
    return result;
}

bool FmodAudioSystem::IsEvent3D(void* eventInstance)
{
    if (!eventInstance)
        return false;

    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    instance->getDescription(&eventDescription);
    if (eventDescription)
    {
        bool is3D = false;
        eventDescription->is3D(&is3D);
        return is3D;
    }
    return false;
}

void FmodAudioSystem::SetEventVolumeMultiplier(void* eventInstance, float volumeScale)
{
    if (!eventInstance)
        return;

    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->setVolume(volumeScale);
}

void FmodAudioSystem::SetEventPitchMultiplier(void* eventInstance, float pitch)
{
    if (!eventInstance)
        return;

    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->setPitch(pitch);
}

float FmodAudioSystem::GetEventMinDistance(void* eventInstance)
{
    if (!eventInstance)
        return -1.0f;

    float minDistance;
    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getMinMaxDistance(&minDistance, nullptr);
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getDescription(&eventDescription);
    return minDistance;
}

float FmodAudioSystem::GetEventMaxDistance(void* eventInstance)
{
    if (!eventInstance)
        return -1.0f;

    float maxDistance;
    static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getMinMaxDistance(nullptr, &maxDistance);
    return maxDistance;
}

void FmodAudioSystem::SetEventMaxDistance(void* eventInstance, float maxDistance)
{
    if (!eventInstance)
        return;

    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MAXIMUM_DISTANCE, maxDistance);
}

void FmodAudioSystem::SetEventMinDistance(void* eventInstance, float minDistance)
{
    if (!eventInstance)
        return;

    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    instance->setProperty(FMOD_STUDIO_EVENT_PROPERTY_MINIMUM_DISTANCE, minDistance);
}

void FmodAudioSystem::SetEventParameter(void* eventInstance, const StringView& parameterName, float value)
{
    if (!eventInstance)
        return;

    auto result = static_cast<FMOD::Studio::EventInstance*>(eventInstance)->setParameterByName(
        parameterName.ToStringAnsi().GetText(), value);
    if (result != FMOD_OK)
        FMODLOG(Warning, "Failed to set event parameter {}. Error: {}", parameterName.ToString(),
            String(FMOD_ErrorString(result)));
}

float FmodAudioSystem::GetEventParameter(void* eventInstance, const StringView& parameterName)
{
    if (!eventInstance)
        return -1.0f;

    float value = -1.0f;
    auto result = static_cast<FMOD::Studio::EventInstance*>(eventInstance)->getParameterByName(
        parameterName.ToStringAnsi().GetText(), &value);
    if (result != FMOD_OK)
        FMODLOG(Warning, "Failed to get event parameter {}. Error: {}", parameterName.ToString(),
            String(FMOD_ErrorString(result)));
    return value;
}

float FmodAudioSystem::GetEventLength(void* eventInstance)
{
    if (!eventInstance)
        return -1.0f;

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    instance->getDescription(&eventDescription);

    int length = -1;
    eventDescription->getLength(&length);
    return static_cast<float>(length) * 0.001f;
}

float FmodAudioSystem::GetEventPosition(void* eventInstance)
{
    if (!eventInstance)
        return -1.0f;

    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    int position = 0;
    instance->getTimelinePosition(&position);
    return static_cast<float>(position) * 0.001f;
}

void FmodAudioSystem::SetEventPosition(void* eventInstance, float position)
{
    if (!eventInstance)
        return;
    auto instance = static_cast<FMOD::Studio::EventInstance*>(eventInstance);
    instance->setTimelinePosition(static_cast<int>(position * 1000.0f));
}

void FmodAudioSystem::SetDriver(int index)
{
    _coreSystem->setDriver(index);
}

int FmodAudioSystem::GetDriver()
{
    int index;
    _coreSystem->getDriver(&index);
    return index;
}

void FmodAudioSystem::UpdateDrivers()
{
    FmodAudio::AudioDevices.Clear();
    int numDrivers = 0;
    _coreSystem->getNumDrivers(&numDrivers);
    // Get audio devices
    for (int i = 0; i < numDrivers; i++)
    {
        char name[64];
        FMOD_GUID guid;
        int systemRate;
        FMOD_SPEAKERMODE speakerMode;
        int speakerModeChannels;
        _coreSystem->getDriverInfo(i, name, 64, &guid, &systemRate, &speakerMode, &speakerModeChannels);
        FmodAudioDevice device;
        device.InternalName = StringAnsi(name, 64);
        auto readableName = String(name, 64);
        readableName.Replace(TEXT("Speakers "), TEXT(""));
        readableName.Replace(TEXT("("), TEXT(""));
        readableName.Replace(TEXT(")"), TEXT(""));
        device.Name = readableName.TrimTrailing();
        FmodAudio::AudioDevices.Add(device);
    }

    // Ensure correct device is the active device.
    FmodAudio::SetActiveAudioDevice(GetDriver());
}

void FmodAudioSystem::SetGlobalParameter(const StringView& parameterName, float value)
{
    if (!_studioSystem)
        return;

    auto result = _studioSystem->setParameterByName(parameterName.ToStringAnsi().GetText(), value);
    if (result != FMOD_OK)
        FMODLOG(Warning, "Failed to set global parameter {}. Error: {}", parameterName.ToString(), String(FMOD_ErrorString(result)));
}

float FmodAudioSystem::GetGlobalParameter(const StringView& parameterName)
{
    if (!_studioSystem)
        return -1.0f;

    float value = -1.0f;
    auto result = _studioSystem->getParameterByName(parameterName.ToStringAnsi().GetText(), &value);
    if (result != FMOD_OK)
        FMODLOG(Warning, "Failed to get global parameter {}. Error: {}", parameterName.ToString(), String(FMOD_ErrorString(result)));
    return value;
}

void FmodAudioSystem::SetBusMute(const String& busPath, bool mute)
{
    FMOD::Studio::Bus* bus = nullptr;
    auto result = _studioSystem->getBus(busPath.ToStringAnsi().GetText(), &bus);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get bus at {}, Error: {}", busPath, String(FMOD_ErrorString(result)));
        return;
    }
    bus->setMute(mute);
}

bool FmodAudioSystem::GetBusMute(const String& busPath)
{
    FMOD::Studio::Bus* bus = nullptr;
    auto result = _studioSystem->getBus(busPath.ToStringAnsi().GetText(), &bus);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get bus at {}, Error: {}", busPath, String(FMOD_ErrorString(result)));
        return false;
    }
    bool mute;
    bus->getMute(&mute);
    return mute;
}

void FmodAudioSystem::SetBusVolumeMultiplier(const String& busPath, float volumeScale)
{
    FMOD::Studio::Bus* bus = nullptr;
    auto result = _studioSystem->getBus(busPath.ToStringAnsi().GetText(), &bus);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get bus at {}, Error: {}", busPath, String(FMOD_ErrorString(result)));
        return;
    }
    bus->setVolume(volumeScale);
}

float FmodAudioSystem::GetBusVolumeMultiplier(const String& busPath)
{
    FMOD::Studio::Bus* bus = nullptr;
    auto result = _studioSystem->getBus(busPath.ToStringAnsi().GetText(), &bus);
    if (result != FMOD_OK)
    {
        FMODLOG(Warning, "Failed to get bus at {}, Error: {}", busPath, String(FMOD_ErrorString(result)));
        return -1.0f;
    }
    float volume;
    bus->getVolume(&volume);
    return volume;
}
