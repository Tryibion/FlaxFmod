using System.IO;
using Flax.Build;
using Flax.Build.NativeCpp;

public class FlaxFmodEditor : GameEditorModule
{
    public override void Init()
    {
        base.Init();
        
        BuildNativeCode = false;
    }

    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);
        
        options.ScriptingAPI.IgnoreMissingDocumentationWarnings = true;

        // Reference game source module to access game code types
        options.PublicDependencies.Add(nameof(FlaxFmod));
    }
}
