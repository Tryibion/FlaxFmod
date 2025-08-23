using System;
using System.Collections.Generic;
using FlaxEditor.CustomEditors;
using FlaxEditor.CustomEditors.Editors;
using FlaxEngine;

namespace FlaxFmod.Editor;

/// <summary>
/// FmodBusEditor class.
/// </summary>
[CustomEditor(typeof(FmodBus))]
public class FmodBusEditor : GenericEditor
{
    /// <inheritdoc />
    public override void Initialize(LayoutElementsContainer layout)
    {
        var labelElement = layout.Label("This Fmod Bus is auto generated. Please do not edit it manually.");
        labelElement.Label.HorizontalAlignment = TextAlignment.Center;
        base.Initialize(layout);
    }
}
