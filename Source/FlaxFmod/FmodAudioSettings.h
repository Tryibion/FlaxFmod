#pragma once

#include "Engine/Core/Config.h"
#include "Engine/Core/Config/Settings.h"
#include "Engine/Core/Collections/Array.h"
#include "Engine/Core/Types/String.h"

API_CLASS() class FLAXFMOD_API FmodAudioSettings : public SettingsBase
{
    API_AUTO_SERIALIZATION();
    DECLARE_SCRIPTING_TYPE_MINIMAL(FmodAudioSettings);
    DECLARE_SETTINGS_GETTER(FmodAudioSettings);
public:
    // Global settings

    /// <summary>
    /// Whether to load all the banks that are located in the bank path.
    /// </summary>
    API_FIELD() bool LoadAllBanks = true;

    /// <summary>
    /// The name of the master bank.
    /// </summary>
    API_FIELD() String MasterBankName = TEXT("Master");

    /// <summary>
    /// An array of the banks to preload. Only used if LoadAllBanks is false.
    /// </summary>
    API_FIELD() Array<String> BanksToPreload;

    /// <summary>
    /// The FMOD folder used for storing banks in the editor and generating assets.
    /// This is relative to the project folder.
    /// </summary>
    API_FIELD() String EditorBankRelativeFolderPath = TEXT("Content/FMOD");

    /// <summary>
    /// The FMOD folder used for storing banks in a built game and generating assets.
    /// This is relative to the startup folder.
    /// </summary>
    API_FIELD() String BuiltProjectBankRelativeFolderPath = TEXT("Content/fmod");

    /// <summary>
    /// The folder or file where the .fspro project is located.
    /// This is relative to the project folder.
    /// </summary>
    API_FIELD() String FmodStudioRelativeProjectPath = TEXT("FMOD"); // Todo: make this a file path editor

    /// <summary>
    /// Editor Only. The environment variable where Fmod studio location is stored. Absolute path will be used if this does not exist.
    /// </summary>
    API_FIELD() String FmodStudioLocationEnvironmentVariable = TEXT("FMOD_STUDIO");

    /// <summary>
    /// Editor Only. The absolute install location of fmod studio. Used if the environment variable is not found.
    /// </summary>
    API_FIELD() String FmodStudioInstallLocation = TEXT("C:\\Program Files/FMOD SoundSystem/FMOD Studio 2.03.08"); // Todo: make this a file path editor

    // Init settings

    /// <summary>
    /// The max channels that can be used with the FMOD system when initialized.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Init\"), Limit(0)") int MaxChannels = 512;
};
