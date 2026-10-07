using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Flax.Build;
using Flax.Build.NativeCpp;

public class FlaxFmod : GameModule
{
    public override void Init()
    {
        base.Init();
        BuildNativeCode = true;
    }

    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);

        options.ScriptingAPI.IgnoreMissingDocumentationWarnings = true;
        
        var fmodPath = Path.Combine(FolderPath, "..", "Fmod");
        var coreIncludePath = Path.Combine(fmodPath, "Include", "core");
        options.CompileEnv.IncludePaths.Add(coreIncludePath);
        var studioIncludePath = Path.Combine(fmodPath, "Include", "studio");
        options.CompileEnv.IncludePaths.Add(studioIncludePath);

        switch (options.Platform.Target)
        {
            case TargetPlatform.Windows:
                // FMod Core Library
                var winCoreLibPath = Path.Combine(fmodPath, "Windows", "core", "x64");
                options.Libraries.Add(Path.Combine(winCoreLibPath, "fmod_vc.lib"));
                options.Libraries.Add(Path.Combine(winCoreLibPath, "fmodL_vc.lib"));
                options.DependencyFiles.Add(Path.Combine(winCoreLibPath, "fmod.dll"));
                options.DependencyFiles.Add(Path.Combine(winCoreLibPath, "fmodL.dll"));
        
                // FMod Studio Library
                var winStudioLibPath = Path.Combine(fmodPath, "Windows", "studio", "x64");
                options.Libraries.Add(Path.Combine(winStudioLibPath, "fmodstudio_vc.lib"));
                options.Libraries.Add(Path.Combine(winStudioLibPath, "fmodstudioL_vc.lib"));
                options.DependencyFiles.Add(Path.Combine(winStudioLibPath, "fmodstudio.dll"));
                options.DependencyFiles.Add(Path.Combine(winStudioLibPath, "fmodstudioL.dll"));
                break;
            case TargetPlatform.Linux:
                // FMod Core Library
                var linCoreLibPath = Path.Combine(fmodPath, "Linux", "core");

                var coreLib = FindVersionedSo(linCoreLibPath, "libfmod");
                var coreLibL = FindVersionedSo(linCoreLibPath, "libfmodL");

                options.DependencyFiles.Add(Path.Combine(linCoreLibPath, coreLib));
                options.DependencyFiles.Add(Path.Combine(linCoreLibPath, coreLibL));
                // Hack for versioned so files. Add "." after version for library
                options.Libraries.Add(Path.Combine(linCoreLibPath, coreLib));
                options.Libraries.Add(Path.Combine(linCoreLibPath, coreLibL));

                // FMod Studio Library
                var linStudioLibPath = Path.Combine(fmodPath, "Linux", "studio");

                var studioLib = FindVersionedSo(linStudioLibPath, "libfmodstudio");
                var studioLibL = FindVersionedSo(linStudioLibPath, "libfmodstudioL");

                options.DependencyFiles.Add(Path.Combine(linStudioLibPath, studioLib));
                options.DependencyFiles.Add(Path.Combine(linStudioLibPath, studioLibL));
                // Hack for versioned so files. Add "." after version for library
                options.Libraries.Add(Path.Combine(linStudioLibPath, studioLib));
                options.Libraries.Add(Path.Combine(linStudioLibPath, studioLibL));
                break;
            default:
                break;
        }
    }

    // Find the .so version files name
    string FindVersionedSo(string directory, string baseName)
    {
        var files = Directory.GetFiles(directory, $"{baseName}.so.*");

        if (files.Length is 0)
            throw new Exception($"{baseName} libraries not found in: {directory}");

        if (files.Length is 1)
            return Path.GetFileName(files[0]);

        var latestVersion = files.OrderByDescending(f =>
        {
            var versionPart = Path.GetFileName(f)
            .Replace($"{baseName}.so.", "");

            var parts = versionPart.Split('.')
            .Select(p => int.TryParse(p, out int n) ? n : 0)
            .ToArray();
            
            return parts.Length >= 2 ? 
            parts[0] * 10000 + parts[1] 
            : 0;

        }).First();

        return Path.GetFileName(latestVersion);
    }

    /// <inheritdoc />
    public override void GetFilesToDeploy(List<string> files)
    {
        // Deploy license
        var licenseFilePath = Path.Combine("..", "Fmod", "LICENSE.TXT");
        if (File.Exists(Path.Combine(FolderPath, licenseFilePath)))
            files.Add(Path.Combine(FolderPath, licenseFilePath));
    }
}
