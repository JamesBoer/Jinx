using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia;
using Avalonia.Platform;
using Avalonia.Layout;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.VisualTree;
using AvaloniaEdit;
using JinxPad.ViewModels;
using System.Diagnostics;
using System.Reflection;
using System;
using System.Threading.Tasks;

namespace JinxPad.Views;

public partial class MainWindow : Window
{
    private TextEditor? editor;

    public MainWindow()
    {
        InitializeComponent();
        Opened += OnOpened;
        Closing += OnClosing;
    }

    private void OnOpened(object? sender, EventArgs e)
    {
        editor = this.FindDescendantOfType<TextEditor>();
        WireCommands();
    }

    private MainViewModel? ViewModel => DataContext as MainViewModel;

    private void WireCommands()
    {
        var newMenuItem = this.FindControl<MenuItem>("NewMenuItem");
        var openMenuItem = this.FindControl<MenuItem>("OpenMenuItem");
        var saveMenuItem = this.FindControl<MenuItem>("SaveMenuItem");
        var saveAsMenuItem = this.FindControl<MenuItem>("SaveAsMenuItem");
        var undoMenuItem = this.FindControl<MenuItem>("UndoMenuItem");
        var redoMenuItem = this.FindControl<MenuItem>("RedoMenuItem");
        var cutMenuItem = this.FindControl<MenuItem>("CutMenuItem");
        var copyMenuItem = this.FindControl<MenuItem>("CopyMenuItem");
        var pasteMenuItem = this.FindControl<MenuItem>("PasteMenuItem");
        var selectAllMenuItem = this.FindControl<MenuItem>("SelectAllMenuItem");
        var exitMenuItem = this.FindControl<MenuItem>("ExitMenuItem");
        var jinxWebsiteMenuItem = this.FindControl<MenuItem>("JinxWebsiteMenuItem");
        var aboutMenuItem = this.FindControl<MenuItem>("AboutMenuItem");

        var newButton = this.FindControl<Button>("NewButton");
        var openButton = this.FindControl<Button>("OpenButton");
        var saveButton = this.FindControl<Button>("SaveButton");
        var undoButton = this.FindControl<Button>("UndoButton");
        var redoButton = this.FindControl<Button>("RedoButton");
        var cutButton = this.FindControl<Button>("CutButton");
        var copyButton = this.FindControl<Button>("CopyButton");
        var pasteButton = this.FindControl<Button>("PasteButton");

        if (newMenuItem is not null) newMenuItem.Click += OnNewClicked;
        if (openMenuItem is not null) openMenuItem.Click += OnOpenClicked;
        if (saveMenuItem is not null) saveMenuItem.Click += OnSaveClicked;
        if (saveAsMenuItem is not null) saveAsMenuItem.Click += OnSaveAsClicked;
        if (undoMenuItem is not null) undoMenuItem.Click += OnUndoClicked;
        if (redoMenuItem is not null) redoMenuItem.Click += OnRedoClicked;
        if (cutMenuItem is not null) cutMenuItem.Click += OnCutClicked;
        if (copyMenuItem is not null) copyMenuItem.Click += OnCopyClicked;
        if (pasteMenuItem is not null) pasteMenuItem.Click += OnPasteClicked;
        if (selectAllMenuItem is not null) selectAllMenuItem.Click += OnSelectAllClicked;
        if (exitMenuItem is not null) exitMenuItem.Click += OnExitClicked;
        if (jinxWebsiteMenuItem is not null) jinxWebsiteMenuItem.Click += OnJinxWebsiteClicked;
        if (aboutMenuItem is not null) aboutMenuItem.Click += OnAboutClicked;

        if (newButton is not null) newButton.Click += OnNewClicked;
        if (openButton is not null) openButton.Click += OnOpenClicked;
        if (saveButton is not null) saveButton.Click += OnSaveClicked;
        if (undoButton is not null) undoButton.Click += OnUndoClicked;
        if (redoButton is not null) redoButton.Click += OnRedoClicked;
        if (cutButton is not null) cutButton.Click += OnCutClicked;
        if (copyButton is not null) copyButton.Click += OnCopyClicked;
        if (pasteButton is not null) pasteButton.Click += OnPasteClicked;
    }

    private async void OnNewClicked(object? sender, RoutedEventArgs e) => await (ViewModel?.NewAsync() ?? Task.CompletedTask);
    private async void OnOpenClicked(object? sender, RoutedEventArgs e) => await (ViewModel?.OpenAsync(this) ?? Task.CompletedTask);
    private async void OnSaveClicked(object? sender, RoutedEventArgs e) => await (ViewModel?.SaveAsync(this) ?? Task.CompletedTask);
    private async void OnSaveAsClicked(object? sender, RoutedEventArgs e) => await (ViewModel?.SaveAsAsync(this) ?? Task.CompletedTask);

    private void OnUndoClicked(object? sender, RoutedEventArgs e) => editor?.Undo();
    private void OnRedoClicked(object? sender, RoutedEventArgs e) => editor?.Redo();
    private void OnCutClicked(object? sender, RoutedEventArgs e) => editor?.Cut();
    private void OnCopyClicked(object? sender, RoutedEventArgs e) => editor?.Copy();
    private void OnPasteClicked(object? sender, RoutedEventArgs e) => editor?.Paste();
    private void OnSelectAllClicked(object? sender, RoutedEventArgs e) => editor?.SelectAll();

    private async void OnExitClicked(object? sender, RoutedEventArgs e)
    {
        if (await ConfirmSaveBeforeClosingAsync())
            Close();
    }

    private void OnJinxWebsiteClicked(object? sender, RoutedEventArgs e)
    {
        Process.Start(new ProcessStartInfo
        {
            FileName = "https://jamesboer.github.io/Jinx/",
            UseShellExecute = true
        });
    }

    private async void OnAboutClicked(object? sender, RoutedEventArgs e)
    {
        var assembly = Assembly.GetExecutingAssembly();
        var appVersion = assembly.GetName().Version?.ToString() ?? "Unknown";
        var infoVersion = assembly.GetCustomAttribute<AssemblyInformationalVersionAttribute>()?.InformationalVersion ?? "Unknown";

        var dialog = new Window
        {
            Title = "About JinxPad",
            Width = 560,
            SizeToContent = SizeToContent.Height,
            CanResize = false,
            WindowStartupLocation = WindowStartupLocation.CenterOwner
        };

        var logoAsset = AssetLoader.Open(new Uri("avares://JinxPad/Assets/jinx-logo.ico"));
        var logo = new Image
        {
            Width = 64,
            Height = 64,
            Source = new Bitmap(logoAsset)
        };

        dialog.Content = new StackPanel
        {
            Margin = new Thickness(18),
            Spacing = 14,
            Children =
            {
                new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    Spacing = 12,
                    Children =
                    {
                        logo,
                        new StackPanel
                        {
                            VerticalAlignment = VerticalAlignment.Center,
                            Children =
                            {
                                new TextBlock { Text = "JinxPad", FontSize = 22, FontWeight = FontWeight.SemiBold },
                                new TextBlock { Text = $"Version: {appVersion}" },
                                new TextBlock { Text = $"Build: {infoVersion}" }
                            }
                        }
                    }
                },
                new TextBlock
                {
                    Text = "JinxPad is a lightweight editor for writing and managing Jinx scripts.",
                    TextWrapping = TextWrapping.Wrap
                },
                new TextBlock
                {
                    Text = "Jinx is an embeddable scripting language written in modern C++, designed to be clean, easy to integrate, and suitable for realtime applications.",
                    TextWrapping = TextWrapping.Wrap
                },
                new StackPanel
                {
                    Orientation = Orientation.Horizontal,
                    HorizontalAlignment = HorizontalAlignment.Right,
                    Children =
                    {
                        new Button
                        {
                            Content = "Close",
                            Width = 90
                        }
                    }
                }
            }
        };

        if (dialog.Content is StackPanel content &&
            content.Children[^1] is StackPanel buttonPanel &&
            buttonPanel.Children[0] is Button closeButton)
        {
            closeButton.Click += (_, _) => dialog.Close();
        }

        await dialog.ShowDialog(this);
    }

    private async void OnClosing(object? sender, WindowClosingEventArgs e)
    {
        if (!await ConfirmSaveBeforeClosingAsync())
            e.Cancel = true;
    }

    private async Task<bool> ConfirmSaveBeforeClosingAsync()
    {
        if (ViewModel?.IsDirty != true)
            return true;

        var dialog = new Window
        {
            Title = "Unsaved Changes",
            Width = 500,
            SizeToContent = SizeToContent.Height,
            CanResize = false,
            WindowStartupLocation = WindowStartupLocation.CenterOwner
        };

        var result = new TaskCompletionSource<bool>();

        var yesButton = new Button { Content = "Yes", Width = 80 };
        var noButton = new Button { Content = "No", Width = 80 };
        var cancelButton = new Button { Content = "Cancel", Width = 80 };

        yesButton.Click += async (_, _) =>
        {
            await (ViewModel?.SaveAsync(this) ?? Task.CompletedTask);
            result.TrySetResult(true);
            dialog.Close();
        };
        noButton.Click += (_, _) =>
        {
            result.TrySetResult(true);
            dialog.Close();
        };
        cancelButton.Click += (_, _) =>
        {
            result.TrySetResult(false);
            dialog.Close();
        };
        dialog.Closing += (_, args) =>
        {
            if (!result.Task.IsCompleted)
                result.TrySetResult(false);
        };

        dialog.Content = new StackPanel
        {
            Margin = new Thickness(16),
            Spacing = 16,
            Children =
            {
                new TextBlock
                {
                    Text = "The current Jinx script has unsaved changes. Save before exiting?",
                    TextWrapping = Avalonia.Media.TextWrapping.Wrap
                },
                new StackPanel
                {
                    Orientation = Avalonia.Layout.Orientation.Horizontal,
                    Spacing = 8,
                    Margin = new Thickness(0, 8, 0, 0),
                    HorizontalAlignment = Avalonia.Layout.HorizontalAlignment.Right,
                    Children = { yesButton, noButton, cancelButton }
                }
            }
        };

        await dialog.ShowDialog(this);
        return await result.Task;
    }
}