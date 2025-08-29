using System;
using System.Collections.Generic;
using FlaxEditor.CustomEditors;
using FlaxEditor.CustomEditors.Editors;
using FlaxEngine;

namespace FlaxFmod.Editor;

/// <summary>
/// FmodEventEditor class.
/// </summary>
[CustomEditor(typeof(FmodBank))]
public class FmodBankEditor : GenericEditor
{
    /// <inheritdoc />
    public override void Initialize(LayoutElementsContainer layout)
    {
        var labelElement = layout.Label("This Fmod Bank is auto generated. Please do not edit it manually.");
        labelElement.Label.HorizontalAlignment = TextAlignment.Center;
        base.Initialize(layout);
    }
}
