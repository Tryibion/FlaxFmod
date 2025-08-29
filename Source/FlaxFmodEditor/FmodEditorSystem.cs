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
    private struct FmodEditorAsset
    {
        public string Path;
        public string Guid;
        
        public FmodEditorAsset()
        {
            Path = string.Empty;
            Guid = string.Empty;
        }
    }
    
    private string _settingsPath;
    private JsonAsset _jsonAsset;
    
    private SpriteAtlas _fmodEventIcon;
    private SpriteAtlas _fmodBusIcon;
    private SpriteAtlas _fmodVcaIcon;
    private SpriteAtlas _fmodBankIcon;
    private SpriteAtlas _fmodSnapshotIcon;
    
    private ContextMenu _fModPluginContextMenu;
    private ContextMenuButton _buildButton;
    private ContextMenuButton _openSettingsButton;
    private ContextMenuButton _openFmodProjectButton;
    private ContextMenuButton _generateGuidButton;
    private ContextMenuButton _buildAndGenerateButton;
    
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

        ViewportIconsRenderer.AddCustomIcon(typeof(FmodAudioListener), Content.LoadAsync<Texture>("Plugins/FlaxFmod/Content/Editor/Icons/AudioListener.flax"));
        SceneGraphFactory.CustomNodesTypes.Add(typeof(FmodAudioListener), typeof(FmodAudioListenerNode));

        ViewportIconsRenderer.AddCustomIcon(typeof(FmodAudioSource), Content.LoadAsync<Texture>("Plugins/FlaxFmod/Content/Editor/Icons/AudioSource.flax"));
        SceneGraphFactory.CustomNodesTypes.Add(typeof(FmodAudioSource), typeof(FmodAudioSourceNode));

        _fmodEventIcon = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD Event Image.flax");
        var eventLogoSprite = _fmodEventIcon.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodEvent>(eventLogoSprite));

        _fmodBusIcon = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD Bus Image.flax");
        var busLogoSprite = _fmodBusIcon.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodBus>(busLogoSprite));

        _fmodVcaIcon = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD VCA Image.flax");
        var vcaLogoSprite = _fmodVcaIcon.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodVca>(vcaLogoSprite));

        _fmodBankIcon = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD Bank Image.flax");
        var bankLogoSprite = _fmodBankIcon.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodBank>(bankLogoSprite));
        
        _fmodSnapshotIcon = Content.Load<SpriteAtlas>("Plugins/FlaxFmod/Content/Editor/Icons/FMOD Snapshot Image.flax");
        var snapshotLogoSprite = _fmodSnapshotIcon.FindSprite("Default");
        Editor.ContentDatabase.AddProxy(new SpawnableJsonAssetProxy<FmodSnapshot>(snapshotLogoSprite));

        Editor.ContentDatabase.Rebuild(true);

        // Menu Options
        var pluginsButton = Editor.UI.MainMenu.GetOrAddButton("Plugins");
        _fModPluginContextMenu = pluginsButton.ContextMenu.GetOrAddChildMenu("Fmod Plugin").ContextMenu;
        _openFmodProjectButton = _fModPluginContextMenu.AddButton("Open Fmod Project", OpenFmodProject);
        _buildButton = _fModPluginContextMenu.AddButton("Build Fmod Project", BuildFmodProject);
        _generateGuidButton = _fModPluginContextMenu.AddButton("Generate Guid Assets", GenerateFmodGuidAssets);
        _buildAndGenerateButton = _fModPluginContextMenu.AddButton("Build Project and Generate Assets", BuildAndGenerate);
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
            var editorBankFolderPath = Path.Combine(Globals.ProjectFolder, settings.FmodStudioRelativeProjectPath, "Build", "Desktop");
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
    }

    private void BuildAndGenerate()
    {
        BuildFmodProject();
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
            {
                // Try to find executable in directory.
                var foundStudio = Directory.GetFiles(studioPath, "fmodstudio*", SearchOption.TopDirectoryOnly);
                if (foundStudio.Length == 0)
                {
                    FlaxEditor.Editor.LogWarning("FMOD studio executable does not exist at path.");
                    return false;
                }
                studioPath = foundStudio[0];
            }
            
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

        // Create assets
        CreateBankAssets(settings, studioProjectPath);
        CreateEventAssets(settings, studioProjectPath);
        CreateBusAssets(settings, studioProjectPath);
        GenerateSnapshotAssets(settings, studioProjectPath);
        CreateVCAAssets(settings, studioProjectPath);
    }

    private void GenerateSnapshotAssets(FmodAudioSettings settings, string studioProjectPath)
    {
         var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var snapshotsFilePath = Path.Combine(studioProjectDirectory, "fmod_snapshots_export.json");
        if (File.Exists(snapshotsFilePath))
        {
            // Create individual files for each Snapshot.
            var fileInfo = File.ReadAllText(snapshotsFilePath);
            var events = JsonSerializer.Deserialize<List<FmodEditorAsset>>(fileInfo);
            var snapshotFolder = Path.Combine(Globals.ProjectFolder, settings.EditorStorageRelativeFolderPath, "Snapshots");
            if (!Directory.Exists(snapshotFolder))
                Directory.CreateDirectory(snapshotFolder);
            
            // Get existing assets.
            Dictionary<string, JsonAsset> existingAssets = new Dictionary<string, JsonAsset>(); // FMOD Guid, Asset
            List<JsonAsset> jsonAssetRemovalList = new List<JsonAsset>();
            var assetIds = Content.GetAllAssetsByType(typeof(FmodSnapshot));
            foreach (var assetId in assetIds)
            {
                var asset = Content.Load<JsonAsset>(assetId);
                if (asset != null)
                {
                    var snapshotInstance = asset.GetInstance<FmodSnapshot>();
                    existingAssets.Add(snapshotInstance.Guid, asset);
                    jsonAssetRemovalList.Add(asset);
                }
            }

            // Rebuild assets
            foreach (var evt in events)
            {
                var relativesnapshotPath = evt.Path.Replace("snapshot:/", "");
                relativesnapshotPath += ".json";
                var savePath = StringUtils.NormalizePath(Path.Combine(snapshotFolder, relativesnapshotPath));
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

                        var instance = asset.GetInstance<FmodSnapshot>();
                        instance.Path = evt.Path;
                        Content.RenameAsset(asset.Path, savePath);
                    }
                }
                else
                {
                    // Create new asset.
                    if (!Directory.Exists(saveFolder))
                        Directory.CreateDirectory(saveFolder);

                    FmodSnapshot fmodSnapshot = new FmodSnapshot
                    {
                        Path = evt.Path,
                        Guid = evt.Guid,
                    };
                    FlaxEditor.Editor.SaveJsonAsset(savePath, fmodSnapshot);
                }
            }
            
            // Remove not reference json assets.
            foreach (var asset in jsonAssetRemovalList)
            {
                Content.DeleteAsset(asset);
            }
            
            // Remove empty directories.
            RemoveEmptyDirectories(snapshotFolder);
        }
    }

    private void CreateBankAssets(FmodAudioSettings settings, string studioProjectPath)
    {
        var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var busFilePath = Path.Combine(studioProjectDirectory, "fmod_banks_export.json");
        if (File.Exists(busFilePath))
        {
            // Create individual files for each bank.
            var fileInfo = File.ReadAllText(busFilePath);
            var banks = JsonSerializer.Deserialize<List<FmodEditorAsset>>(fileInfo);
            var bankFolder = Path.Combine(Globals.ProjectFolder, settings.EditorStorageRelativeFolderPath, "Banks");
            if (!Directory.Exists(bankFolder))
                Directory.CreateDirectory(bankFolder);
            
            // Get existing assets.
            Dictionary<string, JsonAsset> existingAssets = new Dictionary<string, JsonAsset>(); // FMOD Guid, Asset
            List<JsonAsset> jsonAssetRemovalList = new List<JsonAsset>();
            var assetIds = Content.GetAllAssetsByType(typeof(FmodBank));
            foreach (var assetId in assetIds)
            {
                var asset = Content.Load<JsonAsset>(assetId);
                if (asset != null)
                {
                    var bankInstance = asset.GetInstance<FmodBank>();
                    existingAssets.Add(bankInstance.Guid, asset);
                    jsonAssetRemovalList.Add(asset);
                }
            }

            // Rebuild assets
            foreach (var bank in banks)
            {
                var relativeBankPath = bank.Path.Replace("bank:/", "");
                relativeBankPath += ".json";
                var savePath = StringUtils.NormalizePath(Path.Combine(bankFolder, relativeBankPath));
                var saveFolder =  Path.GetDirectoryName(savePath);
                
                if (existingAssets.ContainsKey(bank.Guid))
                {
                    var asset = existingAssets[bank.Guid];
 
                    // Don't remove asset because it exists.
                    jsonAssetRemovalList.Remove(asset);
                    
                    // Check path to ensure the correct path. Move if not correct.
                    if (!asset.Path.Equals(savePath, StringComparison.OrdinalIgnoreCase))
                    {
                        if (!Directory.Exists(saveFolder))
                            Directory.CreateDirectory(saveFolder);

                        var bankInstance = asset.GetInstance<FmodBank>();
                        bankInstance.Path = bank.Path;
                        Content.RenameAsset(asset.Path, savePath);
                    }
                }
                else
                {
                    // Create new asset.
                    if (!Directory.Exists(saveFolder))
                        Directory.CreateDirectory(saveFolder);

                    FmodBank fmodBank = new FmodBank
                    {
                        Path = bank.Path,
                        Guid = bank.Guid,
                    };
                    FlaxEditor.Editor.SaveJsonAsset(savePath, fmodBank);
                }
            }
            
            // Remove not reference json assets.
            foreach (var asset in jsonAssetRemovalList)
            {
                Content.DeleteAsset(asset);
            }
            
            // Remove empty directories.
            RemoveEmptyDirectories(bankFolder);
        }
    }

    private void CreateVCAAssets(FmodAudioSettings settings, string studioProjectPath)
    {
        var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var vcaFilePath = Path.Combine(studioProjectDirectory, "fmod_vca_export.json");
        if (File.Exists(vcaFilePath))
        {
            // Create individual files for each vca.
            var fileInfo = File.ReadAllText(vcaFilePath);
            var vcas = JsonSerializer.Deserialize<List<FmodEditorAsset>>(fileInfo);
            var vcaFolder = Path.Combine(Globals.ProjectFolder, settings.EditorStorageRelativeFolderPath, "VCAs");
            if (!Directory.Exists(vcaFolder))
                Directory.CreateDirectory(vcaFolder);
            
            // Get existing assets.
            Dictionary<string, JsonAsset> existingAssets = new Dictionary<string, JsonAsset>(); // FMOD Guid, Asset
            List<JsonAsset> jsonAssetRemovalList = new List<JsonAsset>();
            var assetIds = Content.GetAllAssetsByType(typeof(FmodVca));
            foreach (var assetId in assetIds)
            {
                var asset = Content.Load<JsonAsset>(assetId);
                if (asset != null)
                {
                    var vcaInstance = asset.GetInstance<FmodVca>();
                    existingAssets.Add(vcaInstance.Guid, asset);
                    jsonAssetRemovalList.Add(asset);
                }
            }

            // Rebuild assets
            foreach (var vca in vcas)
            {
                var relativeVCAPath = vca.Path.Replace("vca:/", "");
                relativeVCAPath += ".json";
                var savePath = StringUtils.NormalizePath(Path.Combine(vcaFolder, relativeVCAPath));
                var saveFolder =  Path.GetDirectoryName(savePath);
                
                if (existingAssets.ContainsKey(vca.Guid))
                {
                    var asset = existingAssets[vca.Guid];
 
                    // Don't remove asset because it exists.
                    jsonAssetRemovalList.Remove(asset);
                    
                    // Check path to ensure the correct path. Move if not correct.
                    if (!asset.Path.Equals(savePath, StringComparison.OrdinalIgnoreCase))
                    {
                        if (!Directory.Exists(saveFolder))
                            Directory.CreateDirectory(saveFolder);

                        var vcaInstance = asset.GetInstance<FmodVca>();
                        vcaInstance.Path = vca.Path;
                        Content.RenameAsset(asset.Path, savePath);
                    }
                }
                else
                {
                    // Create new asset.
                    if (!Directory.Exists(saveFolder))
                        Directory.CreateDirectory(saveFolder);

                    FmodVca fmodVca = new FmodVca
                    {
                        Path = vca.Path,
                        Guid = vca.Guid,
                    };
                    FlaxEditor.Editor.SaveJsonAsset(savePath, fmodVca);
                }
            }
            
            // Remove not reference json assets.
            foreach (var asset in jsonAssetRemovalList)
            {
                Content.DeleteAsset(asset);
            }
            
            // Remove empty directories.
            RemoveEmptyDirectories(vcaFolder);
        }
    }

    private void CreateBusAssets(FmodAudioSettings settings, string studioProjectPath)
    {
        var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var busFilePath = Path.Combine(studioProjectDirectory, "fmod_bus_export.json");
        if (File.Exists(busFilePath))
        {
            // Create individual files for each bus.
            var fileInfo = File.ReadAllText(busFilePath);
            var buses = JsonSerializer.Deserialize<List<FmodEditorAsset>>(fileInfo);
            var busFolder = Path.Combine(Globals.ProjectFolder, settings.EditorStorageRelativeFolderPath, "Buses");
            if (!Directory.Exists(busFolder))
                Directory.CreateDirectory(busFolder);
            
            // Get existing assets.
            Dictionary<string, JsonAsset> existingAssets = new Dictionary<string, JsonAsset>(); // FMOD Guid, Asset
            List<JsonAsset> jsonAssetRemovalList = new List<JsonAsset>();
            var assetIds = Content.GetAllAssetsByType(typeof(FmodBus));
            foreach (var assetId in assetIds)
            {
                var asset = Content.Load<JsonAsset>(assetId);
                if (asset != null)
                {
                    var busInstance = asset.GetInstance<FmodBus>();
                    //Debug.Log($"asset Path: {asset.Path} Bus: {eventInstance.Path}, Guid: {eventInstance.Guid}");
                    existingAssets.Add(busInstance.Guid, asset);
                    jsonAssetRemovalList.Add(asset);
                }
            }

            // Rebuild assets
            foreach (var bus in buses)
            {
                var relativeBusPath = bus.Path.Replace("bus:/", "");
                if (string.IsNullOrEmpty(relativeBusPath))
                    relativeBusPath = "Master";
                relativeBusPath += ".json";
                var savePath = StringUtils.NormalizePath(Path.Combine(busFolder, relativeBusPath));
                var saveFolder =  Path.GetDirectoryName(savePath);
                
                if (existingAssets.ContainsKey(bus.Guid))
                {
                    var asset = existingAssets[bus.Guid];
 
                    // Don't remove asset because it exists.
                    jsonAssetRemovalList.Remove(asset);
                    
                    // Check path to ensure the correct path. Move if not correct.
                    if (!asset.Path.Equals(savePath, StringComparison.OrdinalIgnoreCase))
                    {
                        if (!Directory.Exists(saveFolder))
                            Directory.CreateDirectory(saveFolder);

                        var busInstance = asset.GetInstance<FmodBus>();
                        busInstance.Path = bus.Path;
                        Content.RenameAsset(asset.Path, savePath);
                    }
                }
                else
                {
                    // Create new asset.
                    if (!Directory.Exists(saveFolder))
                        Directory.CreateDirectory(saveFolder);

                    FmodBus fmodBus = new FmodBus
                    {
                        Path = bus.Path,
                        Guid = bus.Guid,
                    };
                    FlaxEditor.Editor.SaveJsonAsset(savePath, fmodBus);
                }
            }
            
            // Remove not reference json assets.
            foreach (var asset in jsonAssetRemovalList)
            {
                Content.DeleteAsset(asset);
            }
            
            // Remove empty directories.
            RemoveEmptyDirectories(busFolder);
        }
    }

    private void CreateEventAssets(FmodAudioSettings settings, string studioProjectPath)
    {
        var studioProjectDirectory = Path.GetDirectoryName(studioProjectPath);
        var eventsFilePath = Path.Combine(studioProjectDirectory, "fmod_events_export.json");
        if (File.Exists(eventsFilePath))
        {
            // Create individual files for each event.
            var fileInfo = File.ReadAllText(eventsFilePath);
            var events = JsonSerializer.Deserialize<List<FmodEditorAsset>>(fileInfo);
            var eventFolder = Path.Combine(Globals.ProjectFolder, settings.EditorStorageRelativeFolderPath, "Events");
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
        _buildAndGenerateButton.Dispose();
        _buildAndGenerateButton = null;
        _fModPluginContextMenu = null;
        
        Editor.PlayModeBegin -= OnPlayModeBegin;
        Editor.PlayModeEnd -= OnPlayModeEnd;
        GameCooker.DeployFiles -= OnDeployFiles;
        
        SceneGraphFactory.CustomNodesTypes.Remove(typeof(FmodAudioListener));
        SceneGraphFactory.CustomNodesTypes.Remove(typeof(FmodAudioSource));
        Content.UnloadAsset(_fmodBusIcon);
        Content.UnloadAsset(_fmodEventIcon);
        Content.UnloadAsset(_fmodVcaIcon);
        Content.UnloadAsset(_fmodBankIcon);
        Content.UnloadAsset(_fmodSnapshotIcon);
        Content.UnloadAsset(_jsonAsset);
    }
}