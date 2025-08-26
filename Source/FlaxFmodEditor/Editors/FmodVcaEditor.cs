using System;
using System.Collections.Generic;
using FlaxEditor.CustomEditors;
using FlaxEditor.CustomEditors.Editors;
using FlaxEngine;

namespace FlaxFmod.Editor;

/// <summary>
/// FmodVCAEditor class.
/// </summary>
[CustomEditor(typeof(FmodVca))]
public class FmodVcaEditor : GenericEditor
{
    /// <inheritdoc />
    public override void Initialize(LayoutElementsContainer layout)
    {
        var labelElement = layout.Label("This Fmod VCA is auto generated. Please do not edit it manually.");
        labelElement.Label.HorizontalAlignment = TextAlignment.Center;
        base.Initialize(layout);
    }
}
