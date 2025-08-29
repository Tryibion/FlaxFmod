using System.IO;
using System.Linq;
using FlaxEditor.CustomEditors;
using FlaxEditor.CustomEditors.Elements;
using FlaxEngine;
using FlaxEngine.GUI;

namespace FlaxFmod.Editor;

/// <summary>
/// Default implementation of the inspector used to edit folder properties.
/// </summary>
public sealed class FolderPathEditor : CustomEditor
{
    private TextBoxElement _element;
    private Window _window;

    /// <inheritdoc />
    public override DisplayStyle Style => DisplayStyle.Inline;

    /// <inheritdoc />
    public override void Initialize(LayoutElementsContainer layout)
    {
        _window = layout.Control.RootWindow.Window;
        var panelElement = layout.CustomContainer<Panel>();
        var panel = panelElement.ContainerControl as Panel;
        
        bool isMultiLine = false;

        var attributes = Values.GetAttributes();
        var multiLine = attributes?.FirstOrDefault(x => x is MultilineTextAttribute);
        if (multiLine != null)
        {
            isMultiLine = true;
        }
        
        _element = panelElement.TextBox(isMultiLine);
        var textBox = _element.TextBox;
        textBox.AnchorPreset = AnchorPresets.StretchAll;
        textBox.Offsets = new Margin(0, 30, 0, 0);
        _element.TextBox.EditEnd += () => SetValue(_element.Text);
        
        panel.Height = 18;
        
        var fileExplorerButton = panelElement.Button("...", "Select a folder from file explorer.");
        var button = fileExplorerButton.Button;
        button.Width = 26;
        button.Height = 18;
        button.AnchorPreset = AnchorPresets.TopRight;
        button.LocalX -= 2;
        button.Clicked += OnClicked;
    }

    private void OnClicked()
    {
        var value = (string)Values[0];
        var initialPath = Globals.ProjectFolder;
        if (!string.IsNullOrEmpty(value))
            initialPath = Path.Combine(Globals.ProjectFolder, value);

        bool folderNotFound = FileSystem.ShowBrowseFolderDialog(_window, initialPath, "Select a folder", out var folderPath);
        if (folderNotFound)
            return;
        
        var path = Path.GetRelativePath(Globals.ProjectFolder, folderPath);
        SetValue(path);
    }

    /// <inheritdoc />
    public override void Refresh()
    {
        base.Refresh();

        if (HasDifferentValues)
        {
            _element.TextBox.Text = string.Empty;
            _element.TextBox.WatermarkText = "Different values";
        }
        else
        {
            _element.TextBox.Text = (string)Values[0];
        }
    }
}