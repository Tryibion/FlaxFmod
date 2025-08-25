using System;
using System.Collections.Generic;
using System.IO;
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
        var coreIncludePath = Path.Combine(FolderPath, "..", "Fmod", "Include", "core");
        options.CompileEnv.IncludePaths.Add(coreIncludePath);
        var studioIncludePath = Path.Combine(FolderPath, "..", "Fmod", "Include", "studio");
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
                options.DependencyFiles.Add(Path.Combine(linCoreLibPath, "libfmod.so.14.8"));
                options.DependencyFiles.Add(Path.Combine(linCoreLibPath, "libfmodL.so.14.8"));
                options.Libraries.Add(Path.Combine(linCoreLibPath, "libfmod.so.14.8"));
                options.Libraries.Add(Path.Combine(linCoreLibPath, "libfmodL.so.14.8"));

                // FMod Studio Library
                var linStudioLibPath = Path.Combine(fmodPath, "Linux", "studio");
                options.DependencyFiles.Add(Path.Combine(linStudioLibPath, "libfmodstudio.so.14.8"));
                options.DependencyFiles.Add(Path.Combine(linStudioLibPath, "libfmodstudioL.so.14.8"));
                options.Libraries.Add(Path.Combine(linStudioLibPath, "libfmodstudio.so.14.8"));
                options.Libraries.Add(Path.Combine(linStudioLibPath, "libfmodstudioL.so.14.8"));
                break;
            default:
                break;
        }
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
