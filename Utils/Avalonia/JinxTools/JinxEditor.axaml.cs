using System;
using System.Reflection;
using System.Xml;
using Avalonia;
using Avalonia.Controls;
using Avalonia.Data;
using AvaloniaEdit;
using AvaloniaEdit.Document;
using AvaloniaEdit.Highlighting;
using AvaloniaEdit.Highlighting.Xshd;

namespace JinxTools;

public partial class JinxEditor : UserControl
{
    public static readonly StyledProperty<string> TextProperty =
        AvaloniaProperty.Register<JinxEditor, string>(nameof(Text), string.Empty, defaultBindingMode: Avalonia.Data.BindingMode.TwoWay);

    public static readonly StyledProperty<IHighlightingDefinition?> SyntaxHighlightingProperty =
        AvaloniaProperty.Register<JinxEditor, IHighlightingDefinition?>(nameof(SyntaxHighlighting), LoadJinxHighlighting());

    private bool isSyncingText;

    public string Text
    {
        get => GetValue(TextProperty);
        set => SetValue(TextProperty, value);
    }

    public IHighlightingDefinition? SyntaxHighlighting
    {
        get => GetValue(SyntaxHighlightingProperty);
        set => SetValue(SyntaxHighlightingProperty, value);
    }

    public JinxEditor()
    {
        InitializeComponent();
        var editor = GetEditor();
        editor.TextChanged += OnEditorTextChanged;
        editor.Text = Text ?? string.Empty;
        editor.SyntaxHighlighting = SyntaxHighlighting;
    }

    // Loads the Jinx syntax highlighting definition (Jinx.xshd) embedded in this assembly.
    private static IHighlightingDefinition? LoadJinxHighlighting()
    {
        var assembly = Assembly.GetExecutingAssembly();
        var resourceName = $"{assembly.GetName().Name}.Jinx.xshd";
        using var stream = assembly.GetManifestResourceStream(resourceName);
        if (stream is null)
            return null;
        using var reader = XmlReader.Create(stream);
        return HighlightingLoader.Load(reader, HighlightingManager.Instance);
    }

    private TextEditor GetEditor() => this.FindControl<TextEditor>("Editor")
                                 ?? throw new InvalidOperationException("Editor control was not found.");

    private void OnEditorTextChanged(object? sender, EventArgs e)
    {
        if (isSyncingText)
            return;

        isSyncingText = true;
        Text = GetEditor().Document?.Text ?? string.Empty;
        isSyncingText = false;
    }

    private void OnBoundTextChanged(string value)
    {
        if (isSyncingText)
            return;

        var editor = GetEditor();
        var newText = value ?? string.Empty;
        if (editor.Document is null)
            editor.Document = new TextDocument(newText);
        else if (editor.Document.Text != newText)
            editor.Document.Text = newText;
    }

    private void OnSyntaxHighlightingChanged(IHighlightingDefinition? highlighting)
    {
        GetEditor().SyntaxHighlighting = highlighting;
    }

    protected override void OnPropertyChanged(AvaloniaPropertyChangedEventArgs change)
    {
        base.OnPropertyChanged(change);

        if (change.Property == TextProperty)
            OnBoundTextChanged(change.GetNewValue<string>());
        else if (change.Property == SyntaxHighlightingProperty)
            OnSyntaxHighlightingChanged(change.GetNewValue<IHighlightingDefinition?>());
    }

}