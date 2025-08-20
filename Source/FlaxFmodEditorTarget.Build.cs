using Flax.Build;

public class FlaxFmodEditorTarget : GameProjectEditorTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        // Reference the modules for editor
        Modules.Add("FlaxFmod");
        Modules.Add("FlaxFmodEditor");
    }
}
