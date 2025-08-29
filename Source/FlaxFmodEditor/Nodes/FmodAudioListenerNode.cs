using System;
using System.Collections.Generic;
using FlaxEditor;
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
 
    /// <inheritdoc />
    public override void OnDebugDraw(ViewportDebugDrawData data)
    {
        base.OnDebugDraw(data);
        if (Engine.IsPlayMode)
        {
            // Draw all sources when listener is selected.
            var sources = FmodAudio.Sources;
            foreach (var source in sources)
            {
                if (!source.OverrideDistance || !source.Is3D())
                    continue;

                BoundingSphere sphere = BoundingSphere.Default;
                sphere.Center = source.Position;
                sphere.Radius = source.MaxDistance;
                DebugDraw.DrawWireSphere(sphere, Color.CornflowerBlue, 0);
                sphere.Radius = source.MinDistance;
                DebugDraw.DrawWireSphere(sphere, Color.CadetBlue, 0);
            }
        }
    }
}

