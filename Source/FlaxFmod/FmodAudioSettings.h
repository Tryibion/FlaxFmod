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
    /// The FMOD folder used for storing banks in a built game and generating assets.
    /// This is relative to the startup folder.
    /// </summary>
    API_FIELD() String BuiltProjectBankRelativeFolderPath = TEXT("Content/fmod");

    /// <summary>
    /// The name of the library related to a fmod plugin. This needs to match the plugin's DLL name.
    /// This will search and use the plugin in the project directory first. If it does not exist it will search in the studio install location.
    /// </summary>
    API_FIELD() Array<String> FmodPluginNames;

    /// <summary>
    /// The FMOD folder used for storing generating assets. Please put this in the Content Folder somewhere.
    /// This is relative to the project folder.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Editor\"), CustomEditorAlias(\"FlaxFmod.Editor.FolderPathEditor\")")
    String EditorStorageRelativeFolderPath = TEXT("Content/FMOD");

    /// <summary>
    /// The folder where the .fspro project is located.
    /// This is relative to the project folder.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Editor\"), CustomEditorAlias(\"FlaxFmod.Editor.FolderPathEditor\")")
    String FmodStudioRelativeProjectPath = TEXT("FMOD");

    /// <summary>
    /// Editor Only. The environment variable where Fmod studio location is stored. Absolute path will be used if this does not exist.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Editor\")") String FmodStudioLocationEnvironmentVariable = TEXT("FMOD_STUDIO");

    /// <summary>
    /// Editor Only. The absolute install location of fmod studio. Used if the environment variable is not found.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Editor\")") String FmodStudioInstallLocation = TEXT("C:\\Program Files/FMOD SoundSystem/FMOD Studio 2.03.08"); // Todo: make this a file path editor

    // Init settings

    /// <summary>
    /// The max channels that can be used with the FMOD system when initialized.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Init\"), Limit(0)") int MaxChannels = 512;

    /// <summary>
    /// Loads the sample data from banks when they are loaded. Increases memory usage.
    /// </summary>
    API_FIELD(Attributes="EditorDisplay(\"Init\")") bool PreloadBankSampleData = true;
};
