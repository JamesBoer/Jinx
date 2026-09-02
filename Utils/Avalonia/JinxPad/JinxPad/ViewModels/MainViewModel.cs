using System;
using System.IO;
using System.Threading.Tasks;
using Avalonia.Controls;
using Avalonia.Platform.Storage;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace JinxPad.ViewModels;

public partial class MainViewModel : ViewModelBase
{
    private bool suppressDirtyTracking;
    private string? lastSavedScriptText;

    [ObservableProperty] public partial string ScriptText { get; set; } = string.Empty;

    [ObservableProperty] public partial string? CurrentFilePath { get; set; }

    [ObservableProperty] public partial string WindowTitle { get; set; } = "JinxPad - Untitled.jinx";
    [ObservableProperty] public partial bool IsDirty { get; set; }

    partial void OnScriptTextChanged(string value)
    {
        if (!suppressDirtyTracking)
            IsDirty = !string.Equals(value, lastSavedScriptText, StringComparison.Ordinal);
    }

    public MainViewModel()
    {
        lastSavedScriptText = ScriptText;
        IsDirty = false;
    }

    public async Task NewAsync()
    {
        SetDocumentState(string.Empty, null);
        await Task.CompletedTask;
    }

    public async Task OpenAsync(Window owner)
    {
        var files = await owner.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open File",
            AllowMultiple = false,
            FileTypeFilter =
            [
                new FilePickerFileType("Jinx Files")
                {
                    Patterns = ["*.jinx"]
                },
                FilePickerFileTypes.All
            ]
        });

        if (files.Count == 0)
            return;

        var file = files[0];
        await using var stream = await file.OpenReadAsync();
        using var reader = new StreamReader(stream);
        var text = await reader.ReadToEndAsync();
        SetDocumentState(text, file.Path.LocalPath);
    }

    public async Task SaveAsync(Window owner)
    {
        if (!string.IsNullOrWhiteSpace(CurrentFilePath))
        {
            await File.WriteAllTextAsync(CurrentFilePath, ScriptText ?? string.Empty);
            MarkSaved();
            return;
        }

        await SaveAsAsync(owner);
    }

    public async Task SaveAsAsync(Window owner)
    {
        var file = await owner.StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save File",
            SuggestedFileName = "Untitled.jinx",
            FileTypeChoices =
            [
                new FilePickerFileType("Jinx Files")
                {
                    Patterns = ["*.jinx"]
                },
                FilePickerFileTypes.All
            ],
            DefaultExtension = "jinx",
            ShowOverwritePrompt = true
        });

        if (file is null)
            return;

        await using var stream = await file.OpenWriteAsync();
        stream.SetLength(0);
        await using var writer = new StreamWriter(stream);
        await writer.WriteAsync(ScriptText ?? string.Empty);

        if (file.Path is not null)
            CurrentFilePath = file.Path.LocalPath;
        MarkSaved();
        UpdateWindowTitle();
    }

    private void SetDocumentState(string text, string? path)
    {
        suppressDirtyTracking = true;
        ScriptText = text;
        CurrentFilePath = path;
        lastSavedScriptText = text;
        IsDirty = false;
        suppressDirtyTracking = false;
        UpdateWindowTitle();
    }

    private void MarkSaved()
    {
        lastSavedScriptText = ScriptText ?? string.Empty;
        IsDirty = false;
    }

    private void UpdateWindowTitle()
    {
        var fileName = string.IsNullOrWhiteSpace(CurrentFilePath)
            ? "Untitled.jinx"
            : Path.GetFileName(CurrentFilePath);
        WindowTitle = $"JinxPad - {fileName}";
    }
}