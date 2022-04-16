using Microsoft.Win32;
using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace JinxPad
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        #region Constructor

        public MainWindow()
        {
            InitializeComponent();
        }

        #endregion

        #region Edit Commands

        #region Cut Command

        void CutCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
        }

        void CutExecuted(object sender, ExecutedRoutedEventArgs e)
        {
        }

        #endregion

        #region Copy Command

        void CopyCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
        }

        void CopyExecuted(object sender, ExecutedRoutedEventArgs e)
        {
        }

        #endregion

        #region Paste Command

        void PasteCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
        }

        void PasteExecuted(object sender, ExecutedRoutedEventArgs e)
        {
        }

        #endregion

        #endregion

        #region Event Handlers

        private void OnInitialized(object sender, EventArgs e)
        {
            // Restore previous window state
            WindowState = Properties.Settings.Default.MainWindowState;
            if (WindowState == System.Windows.WindowState.Minimized)
                WindowState = System.Windows.WindowState.Normal;
            Width = Properties.Settings.Default.MainWindowWidth;
            Height = Properties.Settings.Default.MainWindowHeight;
            Left = Properties.Settings.Default.MainWindowLeft;
            Top = Properties.Settings.Default.MainWindowTop;
        }

        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            m_textChanged = true;
        }

        private void OnNew(object sender, RoutedEventArgs e)
        {
            Editor.Text = "";
            m_textChanged = false;
            m_filePath = String.Empty;
        }

        private void OnOpen(object sender, RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Jinx Files(*.jinx)|*.jinx|All Files(*.*)|*.*";
            dlg.InitialDirectory = Properties.Settings.Default.CurrentFolder;

            bool? success = dlg.ShowDialog();
            if (success.HasValue && success == true)
            {
                try
                {
                    using (StreamReader reader = new StreamReader(dlg.FileName, System.Text.Encoding.UTF8))
                    {
                        Editor.Text = reader.ReadToEnd();
                        m_textChanged = false;
                        m_filePath = dlg.FileName;
                        Properties.Settings.Default.CurrentFolder = Path.GetDirectoryName(dlg.FileName);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error opening Jinx file: " + ex.ToString());
                }
            }
        }

        private void OnSave(object sender, RoutedEventArgs e)
        {
            FileSave();
        }

        private void OnSaveAs(object sender, RoutedEventArgs e)
        {
            FileSaveAs();
        }

        private void OnHelpViewTutorial(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("https://jamesboer.github.io/Jinx/Tutorial.pdf");
        }

        private void OnHelpAbout(object sender, RoutedEventArgs e)
        {
            var aboutDlg = new AboutWindow();
            aboutDlg.ShowDialog();
        }

        private void OnExit(object sender, RoutedEventArgs e)
        {
            Application.Current.MainWindow.Close();
        }

        private void OnClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            e.Cancel = !CheckExit();
        }

        private void OnClosed(object sender, EventArgs e)
        {
            Properties.Settings.Default.Save();
        }

        #endregion

        #region Private Functions

        private void FileSaveAs()
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Jinx Files(*.jinx)|*.jinx|All Files(*.*)|*.*";
            dlg.AddExtension = true;
            dlg.InitialDirectory = Properties.Settings.Default.CurrentFolder;
            bool? success = dlg.ShowDialog();
            if (success.HasValue && success == true)
            {
                m_filePath = dlg.FileName;
                Properties.Settings.Default.CurrentFolder = Path.GetDirectoryName(dlg.FileName);
                FileSave();
            }
        }

        private void FileSave()
        {
            if (String.IsNullOrEmpty(m_filePath))
            {
                FileSaveAs();
            }
            else
            {
                try
                {
                    using (StreamWriter writer = new StreamWriter(m_filePath))
                    {
                        writer.Write(Editor.Text);
                        m_textChanged = false;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error saving Jinx file: " + ex.ToString());
                }
            }
        }

        private bool CheckExit()
        {
            if (m_textChanged)
            {
                var result = MessageBox.Show("There are unsaved changes.\nDo you wish to save the file before closing?", "JinxPad", MessageBoxButton.YesNoCancel);
                if (result == MessageBoxResult.Cancel)
                    return false;
                else if (result == MessageBoxResult.Yes)
                    FileSave();
                return true;
            }
            return true;
        }

        #endregion

        #region Private Members

        private bool m_textChanged;
        private string m_filePath;

        #endregion

    }
}
