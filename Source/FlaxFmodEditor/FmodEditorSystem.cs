using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using FlaxEditor;
using FlaxEditor.Content;
using FlaxEditor.Content.Settings;
using FlaxEditor.GUI.ContextMenu;
using FlaxEditor.SceneGraph;
using FlaxEditor.Windows;
using FlaxEngine;
using FlaxEngine.Json;
using Debug = FlaxEngine.Debug;


namespace FlaxFmod.Editor;

/// <summary>
/// Fmod Editor System
/// </summary>
public class FmodEditorSystem : EditorPlugin
{
    private struct FmodEditorEvent
    {
        public string Path;
        public string Guid;
        
        public FmodEditorEvent()
        {
            Path = string.Empty;
            Guid = string.Empty;
        }
    }
    
    private string _settingsPath;
    private JsonAsset _jsonAsset;
    
    private ContextMenu _fModPluginContextMenu;
    private ContextMenuButton _buildButton;
    private ContextMenuButton _openSettingsButton;
    private ContextMenuButton _openFmodProjectButton;
    private ContextMenuButton _generateGuidButton;
    
    private GameWindow _gameWindow;
    private float _storedMasterVolume;
    private bool _storedMuteAudio;
    
    /// <inheritdoc />
    public override void InitializeEditor()
    {
        // Get/Generate settings asset.
        _settingsPath = Path.Combine(Globals.ProjectContentFolder, "Settings", "FmodAudioSettings.json");
        if (!File.Exists(_settingsPath))
        {
            FlaxEditor.Editor.SaveJsonAsset(_settingsPath, new FmodAudioSettings());
        }
        _jsonAsset = Engine.GetCustomSettings("FmodAudioSettings");
        if (_jsonAsset == null)
        {
            _jsonAsset = Content.LoadAsync<JsonAsset>(_settingsPath);
            GameSettings.SetCustomSettings("FmodAudioSettings", _jsonAsset);
        }
        
        ViewportIconsRenderer.AddCustomIcon(typeof(FmodAudioListener), Content.LoadAsync<Texture>(System.IO.Path.Combine(Globals.EngineContentFolder, "Editor/Icons/Textures/AudioListner.flax")));
        SceneGraphFactory.CustomNodesTypes.Add(typeof(FmodAudioListener), typeof(FmodAudioListenerNode));
        
        ViewportIconsRenderer.AddCustomIcon(typeof(FmodAudioSource), Content.LoadAsync<Texture>(System.IO.Path.Combine(Globals.EngineContentFolder, "Editor/Icons/Textures/AudioSource.flax")));
        SceneGraphFactory.CustomNodesTypes.Add(typeof(FmodAudioSource), typeof(FmodAudioSourceNode));

        var logo = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD Event Image.flax");
        logo.WaitForLoaded();
        var logoSprite = logo.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodEvent>(logoSprite));
        
        // Menu Options
        var pluginsButton = Editor.UI.MainMenu.GetOrAddButton("Plugins");
        _fModPluginContextMenu = pluginsButton.ContextMenu.GetOrAddChildMenu("Fmod Plugin").ContextMenu;
        _openFmodProjectButton = _fModPluginContextMenu.AddButton("Open Fmod Project", OpenFmodProject);
        _buildButton = _fModPluginContextMenu.AddButton("Build Fmod project", BuildFmodProject);
        _generateGuidButton = _fModPluginContextMenu.AddButton("Generate Guid Assets", GenerateFmodGuidAssets);
        _openSettingsButton = _fModPluginContextMenu.AddButton("Open Fmod Settings", OpenSettings);

        Editor.PlayModeBegin += OnPlayModeBegin;
        Editor.PlayModeEnd += OnPlayModeEnd;
        
        GameCooker.DeployFiles += OnDeployFiles;
    }

    private void OnDeployFiles()
    {
        if (_jsonAsset)
        {
            var settings = _jsonAsset.GetInstance<FmodAudioSettings>();
            var builtBankFolderPath = Path.Combine(GameCooker.CurrentData.DataOutputPath, settings.BuiltProjectBankRelativeFolderPath);

            // TODO: Get specific bank folder based on platform.
            var editorBankFolderPath = Path.Combine(Globals.ProjectFolder, settings.EditorBankRelativeFolderPath, "Build", "Desktop");

            var banks = Directory.GetFiles(editorBankFolderPath, "*.bank", SearchOption.AllDirectories);
            
            if (!Directory.Exists(builtBankFolderPath))
                Directory.CreateDirectory(builtBankFolderPath);

            foreach (var bank in banks)
            {
                var relativePath = Path.GetRelativePath(editorBankFolderPath, bank);
                File.Copy(bank, Path.Combine(builtBankFolderPath, relativePath), true);;
            }
        }
    }

    private void OnPlayModeBegin()
    {
        _gameWindow = Editor.Windows.GameWin;
        FmodAudio.SetMasterVolume(_gameWindow.AudioMuted ? 0.0f : _gameWindow.AudioVolume);
        _storedMasterVolume = _gameWindow.AudioVolume;
        _storedMuteAudio = _gameWindow.AudioMuted;
        Editor.EditorUpdate += OnPlayModeUpdate;
    }

    private void OnPlayModeUpdate()
    {
        if (_gameWindow == null)
            return;
        // TODO: change this to events if game window ever gets events for the audio changes.
        if (!Mathf.NearEqual(_gameWindow.AudioVolume, _storedMasterVolume) || _gameWindow.AudioMuted != _storedMuteAudio)
        {
            FmodAudio.SetMasterVolume(_gameWindow.AudioMuted ? 0.0f : _gameWindow.AudioVolume);
            _storedMasterVolume = _gameWindow.AudioVolume;
            _storedMuteAudio = _gameWindow.AudioMuted;
        }
    }
    
    private void OnPlayModeEnd()
    {
        Editor.EditorUpdate -= OnPlayModeUpdate;
    }

    private void OpenFmodProject()
    {
        if (!GetPaths(out _, out var projectPath, out _))
            return;
        
        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = StringUtils.NormalizePath(projectPath),
            CreateNoWindow = true,
#if PLATFORM_WINDOWS
            UseShellExecute = true,
#endif
        };

        FlaxEditor.Editor.Log("Opening Fmod Project.");
        var process = Process.Start(startInfo);
    }

    private void OpenSettings()
    {
        if (_jsonAsset == null)
            return;
        Editor.ContentEditing.Open(_jsonAsset);
    }

    private void BuildFmodProject()
    {
        if (!GetPaths(out var studioPath, out var projectPath, out _))
            return;
        
        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = studioPath,
            Arguments = $"-build {projectPath}",
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardOutput = true,
        };
        FlaxEditor.Editor.Log($"Running FMOD studio build for project {projectPath}.");
        var process = Process.Start(startInfo);
        var output = process?.StandardOutput.ReadToEnd();
        if (!string.IsNullOrEmpty(output))
            FlaxEditor.Editor.Log($"FMOD studio build running. {output}");
        process?.WaitForExit();
        FlaxEditor.Editor.Log("FMOD studio build complete.");
        
        GenerateFmodGuidAssets();
    }

    private bool GetPaths(out string studioPath, out string studioProjectPath, out FmodAudioSettings settings)
    {
        studioPath = null;
        studioProjectPath = null;
        settings = null;
        if (_jsonAsset == null)
        {
            FlaxEditor.Editor.LogWarning("FMOD Audio Settings asset does not exist in custom settings. Please add an FmodAudioSettings asset named \"FmodAudioSettings\" as a custom setting in GameSetting's asset.");
            return false;
        }
        _jsonAsset.WaitForLoaded();

        settings = _jsonAsset.GetInstance<FmodAudioSettings>();
        if (settings == null)
            return false;
        
        // Get studio install path.
        studioPath = Environment.GetEnvironmentVariable(settings.FmodStudioLocationEnvironmentVariable);
        if (string.IsNullOrEmpty(studioPath))
            studioPath = StringUtils.NormalizePath(settings.FmodStudioInstallLocation);
        else
            studioPath = StringUtils.NormalizePath(studioPath);
        studioProjectPath = StringUtils.NormalizePath(Path.Combine(Globals.ProjectFolder, settings.FmodStudioRelativeProjectPath));

        if (Directory.Exists(studioPath))
        {
            var attrib = File.GetAttributes(studioPath);
            if (attrib.HasFlag(FileAttributes.Directory))
                studioPath = Path.Combine(studioPath, "fmodstudio.exe");
            
            if (!File.Exists(studioPath))
            {
                FlaxEditor.Editor.LogWarning("FMOD studio executable does not exist at path.");
                return false;
            }
        }
        else
        {
            FlaxEditor.Editor.LogWarning("FMOD studio install directory doesn't exist.");
            return false;
        }

        // Get studio project path.
        if (Directory.Exists(studioProjectPath))
        {
            var attrib = File.GetAttributes(studioProjectPath);
            if (attrib.HasFlag(FileAttributes.Directory))
            {
                var files = Directory.GetFiles(studioProjectPath, "*.fspro", SearchOption.AllDirectories);
                if (files.Length == 0)
                {
                    FlaxEditor.Editor.LogWarning("FMOD studio project file does not exist at path.");
                    return false;
                }
                studioProjectPath = files[0];
            }
            else if (!Path.GetExtension(studioProjectPath).Contains(".fspro", StringComparison.OrdinalIgnoreCase))
            {
                FlaxEditor.Editor.LogWarning("FMOD studio project file does not exist at path.");
                return false;
            }
        }
        else
        {
            FlaxEditor.Editor.LogWarning("FMOD studio project directory doesn't exist.");
            return false;
        }

        return true;
    }

    private void GenerateFmodGuidAssets()
    {
        if (!GetPaths(out var studioPath, out var studioProjectPath, out var settings))
            return;
        
        // Run Process
        var pathToScript = Path.Combine(Globals.ProjectContentFolder, "..", "Plugins", "FlaxFmod", "Source", "FlaxFmodEditor", "Utilities", "exportGUIDJson.js");
        if (!File.Exists(pathToScript))
        {
            FlaxEditor.Editor.LogWarning("FMOD studio script doesn't exist.");
            return;
        }
        
        ProcessStartInfo startInfo = new ProcessStartInfo
        {
            FileName = studioPath,
            Arguments = $"-script {pathToScript} {studioProjectPath}",
            CreateNoWindow = true,
            UseShellExecute = false,
            RedirectStandardOutput = true,
        };
        FlaxEditor.Editor.Log("Running FMOD studio script to get all events.");
        var process = Process.Start(startInfo);
        var output = process?.StandardOutput.ReadToEnd();
        if (!string.IsNullOrEmpty(output))
            FlaxEditor.Editor.Log($"FMOD studio script running. {output}");
        process?.WaitForExit();
        FlaxEditor.Editor.Log("FMOD studio script completed.");

        // Copy file
        var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var guidFilePath = Path.Combine(studioProjectDirectory, "fmod_events_export.json");
        if (File.Exists(guidFilePath))
        {
            var newGuidLocation = Path.Combine(Globals.ProjectSourceFolder, "FMOD", "fmod_events_export.json");
            Directory.CreateDirectory(Path.GetDirectoryName(newGuidLocation));
            File.Copy(guidFilePath, newGuidLocation, true);
            // TODO: create individual files for each event.
            var fileInfo = File.ReadAllText(newGuidLocation);
            var events = JsonSerializer.Deserialize<List<FmodEditorEvent>>(fileInfo);
            var eventFolder = Path.Combine(Globals.ProjectFolder, settings.EditorBankRelativeFolderPath, "Events");
            if (!Directory.Exists(eventFolder))
                Directory.CreateDirectory(eventFolder);
            
            // Get existing assets.
            Dictionary<string, JsonAsset> existingAssets = new Dictionary<string, JsonAsset>(); // FMOD Guid, Asset
            List<JsonAsset> jsonAssetRemovalList = new List<JsonAsset>();
            var assetIds = Content.GetAllAssetsByType(typeof(FmodEvent));
            foreach (var assetId in assetIds)
            {
                var asset = Content.Load<JsonAsset>(assetId);
                if (asset != null)
                {
                    var eventInstance = asset.GetInstance<FmodEvent>();
                    //Debug.Log($"asset Path: {asset.Path} Event: {eventInstance.Path}, Guid: {eventInstance.Guid}");
                    existingAssets.Add(eventInstance.Guid, asset);
                    jsonAssetRemovalList.Add(asset);
                }
            }

            // Rebuild assets
            foreach (var evt in events)
            {
                var relativeEventPath = evt.Path.Replace("event:/", "");
                relativeEventPath += ".json";
                var savePath = StringUtils.NormalizePath(Path.Combine(eventFolder, relativeEventPath));
                var saveFolder =  Path.GetDirectoryName(savePath);
                
                if (existingAssets.ContainsKey(evt.Guid))
                {
                    var asset = existingAssets[evt.Guid];
 
                    // Don't remove asset because it exists.
                    jsonAssetRemovalList.Remove(asset);
                    
                    // Check path to ensure the correct path. Move if not correct.
                    if (!asset.Path.Equals(savePath, StringComparison.OrdinalIgnoreCase))
                    {
                        if (!Directory.Exists(saveFolder))
                            Directory.CreateDirectory(saveFolder);

                        var eventInstance = asset.GetInstance<FmodEvent>();
                        eventInstance.Path = evt.Path;
                        Content.RenameAsset(asset.Path, savePath);
                    }
                }
                else
                {
                    // Create new asset.
                    if (!Directory.Exists(saveFolder))
                        Directory.CreateDirectory(saveFolder);

                    FmodEvent fmodEvent = new FmodEvent
                    {
                        Path = evt.Path,
                        Guid = evt.Guid,
                    };
                    FlaxEditor.Editor.SaveJsonAsset(savePath, fmodEvent);
                }
            }
            
            // Remove not reference json assets.
            foreach (var asset in jsonAssetRemovalList)
            {
                Content.DeleteAsset(asset);
            }
            
            // Remove empty directories.
            RemoveEmptyDirectories(eventFolder);
        }
    }

    private void RemoveEmptyDirectories(string directory)
    {
        foreach (var subDirectory in Directory.GetDirectories(directory))
        {
            RemoveEmptyDirectories(subDirectory);
            if (!Directory.EnumerateFileSystemEntries(subDirectory).Any())
            {
                Directory.Delete(subDirectory, false);
            }
        }
    }

    /// <inheritdoc />
    public override void DeinitializeEditor()
    {
        _buildButton.Dispose();
        _buildButton = null;
        _openSettingsButton.Dispose();
        _openSettingsButton = null;
        _openFmodProjectButton.Dispose();
        _openFmodProjectButton = null;
        _generateGuidButton.Dispose();
        _generateGuidButton = null;
        _fModPluginContextMenu = null;
        
        Editor.PlayModeBegin -= OnPlayModeBegin;
        Editor.PlayModeEnd -= OnPlayModeEnd;
        GameCooker.DeployFiles -= OnDeployFiles;
        
        SceneGraphFactory.CustomNodesTypes.Remove(typeof(FmodAudioListener));
        SceneGraphFactory.CustomNodesTypes.Remove(typeof(FmodAudioSource));
        Content.UnloadAsset(_jsonAsset);
    }
}