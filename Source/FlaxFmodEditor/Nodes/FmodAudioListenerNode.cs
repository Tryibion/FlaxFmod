using System;
using System.Collections.Generic;
using FlaxEditor.SceneGraph;
using FlaxEngine;

namespace FlaxFmod.Editor;

/// <summary>
/// Scene tree node for <see cref="FmodAudioListenerNode"/> actor type.
/// </summary>
/// <seealso cref="ActorNodeWithIcon" />
[HideInEditor]
public sealed class FmodAudioListenerNode : ActorNodeWithIcon
{
    /// <inheritdoc />
    public FmodAudioListenerNode(Actor actor)
        : base(actor)
    {
    }
}

