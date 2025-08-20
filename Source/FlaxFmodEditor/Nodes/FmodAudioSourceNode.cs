using System;
using System.Collections.Generic;
using FlaxEditor.SceneGraph;
using FlaxEngine;

namespace FlaxFmod.Editor;

/// <summary>
/// Scene tree node for <see cref="FmodAudioSourceNode"/> actor type.
/// </summary>
/// <seealso cref="ActorNodeWithIcon" />
[HideInEditor]
public sealed class FmodAudioSourceNode : ActorNodeWithIcon
{
    /// <inheritdoc />
    public FmodAudioSourceNode(Actor actor)
        : base(actor)
    {
    }
}
