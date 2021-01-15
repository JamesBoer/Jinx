using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace JinxTools
{

    #region VisualHost Helper Class
    public class VisualHost : FrameworkElement
    {
        public VisualHost()
        {
            visual = new DrawingVisual();
            visuals = new VisualCollection(this);
            visuals.Add(visual);
        }

        public DrawingContext RenderOpen()
        {
            return visual.RenderOpen();
        }

        protected override int VisualChildrenCount
        {
            get { return visuals.Count; }
        }

        protected override Visual GetVisualChild(int index)
        {
            if (index < 0 || index >= visuals.Count)
                throw new ArgumentOutOfRangeException();
            return visuals[index];
        }

        private VisualCollection visuals;
        private DrawingVisual visual;

    }

    #endregion

    /// <summary>
    /// Interaction logic for Editor.xaml
    /// </summary>
    public partial class Editor : TextBox
    {
        #region Constructor

        public Editor()
        {
            InitializeComponent();
        }

        #endregion

        #region Properties

        public Visibility LineNumbersVisibility
        {
            get { return (Visibility)GetValue(LineNumberVisibilityProperty); }
            set { SetValue(LineNumberVisibilityProperty, value); }
        }

        public static readonly DependencyProperty LineNumberVisibilityProperty =
            DependencyProperty.Register("LineNumberVisibility", typeof(Visibility), typeof(Editor), new PropertyMetadata(Visibility.Visible));


        public Brush DefaultTextBrush
        {
            get { return (Brush)GetValue(DefaultTextBrushProperty); }
            set { SetValue(DefaultTextBrushProperty, value); }
        }

        public static readonly DependencyProperty DefaultTextBrushProperty =
            DependencyProperty.Register("DefaultTextBrush", typeof(Brush), typeof(Editor), new PropertyMetadata(new SolidColorBrush(Colors.Black)));


        public Brush KeywordTextBrush
        {
            get { return (Brush)GetValue(KeywordTextBrushProperty); }
            set { SetValue(KeywordTextBrushProperty, value); }
        }

        public static readonly DependencyProperty KeywordTextBrushProperty =
            DependencyProperty.Register("KeywordTextBrush", typeof(Brush), typeof(Editor), new PropertyMetadata(new SolidColorBrush((Color)ColorConverter.ConvertFromString("#0070C0"))));


        public Brush ValueTextBrush
        {
            get { return (Brush)GetValue(ValueTextBrushProperty); }
            set { SetValue(ValueTextBrushProperty, value); }
        }

        public static readonly DependencyProperty ValueTextBrushProperty =
            DependencyProperty.Register("ValueTextBrush", typeof(Brush), typeof(Editor), new PropertyMetadata(new SolidColorBrush((Color)ColorConverter.ConvertFromString("#C00000"))));


        public Brush CommentTextBrush
        {
            get { return (Brush)GetValue(CommentTextBrushProperty); }
            set { SetValue(CommentTextBrushProperty, value); }
        }

        public static readonly DependencyProperty CommentTextBrushProperty =
            DependencyProperty.Register("CommentTextBrush", typeof(Brush), typeof(Editor), new PropertyMetadata(new SolidColorBrush((Color)ColorConverter.ConvertFromString("#00B050"))));


        public Brush LineNumbersTextBrush
        {
            get { return (Brush)GetValue(LineNumbersTextBrushProperty); }
            set { SetValue(LineNumbersTextBrushProperty, value); }
        }

        public static readonly DependencyProperty LineNumbersTextBrushProperty =
            DependencyProperty.Register("LineNumbersTextBrush", typeof(Brush), typeof(Editor), new PropertyMetadata(new SolidColorBrush(Colors.DarkGray)));


        public int SpacesPerTab
        {
            get { return (int)GetValue(SpacesPerTabProperty); }
            set { SetValue(SpacesPerTabProperty, value); }
        }

        public static readonly DependencyProperty SpacesPerTabProperty =
            DependencyProperty.Register("SpacesPerTab", typeof(int), typeof(Editor), new PropertyMetadata(4));

        #endregion

        #region Event Handlers

        private void OnScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            InvalidateVisual();
        }

        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            InvalidateVisual();
        }

        private void OnSizeChanged(object sender, SizeChangedEventArgs e)
        {
            InvalidateVisual();
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            InvalidateVisual();
        }

        private void OnLayoutUpdated(object sender, EventArgs e)
        {
            // Line count is only valid after layout has been updated
            m_lineCount = LineCount;
        }

        private void OnPreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            // Code editors typically handle tabs a bit specially
            if (e.Key == System.Windows.Input.Key.Tab)
            {
                // Check for shift modifier key
                if (e.KeyboardDevice.IsKeyDown(Key.LeftShift) || e.KeyboardDevice.IsKeyDown(Key.RightShift))
                {
                    // Remove indentation on shift-tab
                    if (SelectionLength == 0)
                    {
                        int i = SelectionStart - 1;
                        for (; i >= (SelectionStart - SpacesPerTab) && i >= 0; --i)
                        {
                            if (Text[i] != ' ')
                                break;
                        }
                        int count = SelectionStart - i - 1;
                        if (count > 0)
                        {
                            int newStart = SelectionStart - count;
                            StringBuilder s = new StringBuilder(Text);
                            s.Remove(SelectionStart - count, count);
                            Text = s.ToString();
                            SelectionStart = newStart;
                        }
                    }
                }
                else
                {
                    int start = SelectionStart;

                    // Substitute multiple spaces for tab.  Start by building a string of spaces.
                    StringBuilder spaces = new StringBuilder(8);
                    for (int i = 0; i < SpacesPerTab; ++i)
                        spaces.Append(" ");

                    if (SelectionLength == 0)
                    {
                        // No selection indicates simple insert
                        Text = Text.Insert(start, spaces.ToString());
                    }
                    else
                    {
                        // Otherwise, substitute selected text with a tab
                        StringBuilder s = new StringBuilder(Text);
                        s.Remove(start, SelectionLength);
                        s.Insert(start, spaces.ToString());
                        Text = s.ToString();
                    }

                    // Set selection start after text has changed
                    SelectionStart = start + 4;

                }
                // Do nothing else
                e.Handled = true;
            }
        }

        #endregion

        #region Overridden Methods

        /// <summary>
        /// Called after a new template is applied. 
        /// </summary>
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();

            // Override the foreground color.  It needs to be transparent so it doesn't
            // draw over our custom text rendering
            Foreground = new SolidColorBrush(Colors.Transparent);

            // Set line height
            m_lineHeight = FontSize * 1.3;
            TextBlock.SetLineStackingStrategy(this, LineStackingStrategy.BlockLineHeight);
            TextBlock.SetLineHeight(this, m_lineHeight);

            // Retrieve internal template parts
            m_renderContainer = (VisualHost)Template.FindName("PART_RenderCanvas", this);
            m_lineNumbersContainer = (VisualHost)Template.FindName("PART_LineNumbersCanvas", this);
            m_scrollViewer = (ScrollViewer)Template.FindName("PART_ContentHost", this);

            // Add scroll changed event handler
            m_scrollViewer.ScrollChanged += OnScrollChanged;

            // Adjust line number margin width
            SetLineNumberCanvasWidth();

            // Render the control
            InvalidateVisual();
        }

        /// <summary>
        /// Overrides the rendering function to perform custom rendering calls.
        /// </summary>
        /// <param name="drawingContext"></param>
        protected override void OnRender(DrawingContext drawingContext)
        {
            if (!IsLoaded || m_renderContainer == null || m_lineNumbersContainer == null)
                return;

            if (LineCount > 0)
                m_lineCount = LineCount;

            // Calculate beginning and ending line
            m_viewLineStart = (int)Math.Floor(VerticalOffset / m_lineHeight);
            m_viewLineEnd = m_viewLineStart + (int)Math.Ceiling(m_scrollViewer.ActualHeight / m_lineHeight);

            // Draw highlighted text
            var dc = m_renderContainer.RenderOpen();
            var fmtText = GetHighlightedText(GetVisibleLines());
            dc.DrawText(fmtText, new Point(-HorizontalOffset + 2, -VerticalOffset + m_textRenderOffset));
            dc.Close();

            // Adjust line number margin width based on line number count
            SetLineNumberCanvasWidth();

            // Draw line numbers
            dc = m_lineNumbersContainer.RenderOpen();
            fmtText = GetLineNumbers();
            dc.DrawText(fmtText, new Point(-HorizontalOffset + m_lineNumbersContainer.ActualWidth, -VerticalOffset + (m_viewLineStart * m_lineHeight)));
            dc.Close();

            base.OnRender(drawingContext);
        }

        #endregion

        #region Private Methods

        /// <summary>
        /// Create highlighted text from a plain text string
        /// </summary>
        /// <param name="text"></param>
        /// <returns></returns>
        private FormattedText GetHighlightedText(string text)
        {
            // Create formatted text string, so we can apply syntax highlighting
            var fmtTxt = GetFormattedText(text, DefaultTextBrush);

            // Keywords
            Regex wordsRgx = new Regex("[a-zA-Z_][a-zA-Z0-9_]*");
            foreach (Match match in wordsRgx.Matches(text))
            {
                if (m_keywords.Contains(match.Value))
                    fmtTxt.SetForegroundBrush(KeywordTextBrush, match.Index, match.Length);
                else if (m_values.Contains(match.Value))
                    fmtTxt.SetForegroundBrush(ValueTextBrush, match.Index, match.Length);
            }

            // Numbers
            Regex numberRegex = new Regex(@"\w*(?<![\w])[-]?[0-9]*\.?[0-9]+");
            foreach (Match match in numberRegex.Matches(text))
                fmtTxt.SetForegroundBrush(ValueTextBrush, match.Index, match.Length);
            
            // Strings
            Regex stringRegex = new Regex(@"\""([^\""]*)\""");
            foreach (Match match in stringRegex.Matches(text))
                fmtTxt.SetForegroundBrush(ValueTextBrush, match.Index, match.Length);
            
            // Line Comments
            Regex lineCommentRegex = new Regex(@"--.*");
            foreach (Match match in lineCommentRegex.Matches(text))
                fmtTxt.SetForegroundBrush(CommentTextBrush, match.Index, match.Length);

            // Block Comments
            Regex blockCommentRegex = new Regex(@"(?s)-{3,}(.*?)-{3,}");
            foreach (Match match in blockCommentRegex.Matches(text))
                fmtTxt.SetForegroundBrush(CommentTextBrush, match.Index, match.Length);
            
            return fmtTxt;
        }

        /// <summary>
        /// Create line number text based on current line count and view.
        /// </summary>
        /// <returns></returns>
        private FormattedText GetLineNumbers()
        {
            StringBuilder lineNumberText = new StringBuilder();
            for (int i = m_viewLineStart + 1; i <= m_lineCount && i <= m_viewLineEnd; ++i)
            {
                lineNumberText.Append(i.ToString());
                lineNumberText.Append("\n");
            }
            var text = lineNumberText.ToString().Trim();
            var fmtText = GetFormattedText(text, LineNumbersTextBrush);
            fmtText.TextAlignment = TextAlignment.Right;
            return fmtText;
        }

        /// <summary>
        /// Sets the line number canvas width based on current line count.
        /// </summary>
        private void SetLineNumberCanvasWidth()
        {
            m_lineNumbersContainer.Width = GetFormattedText(string.Format("{0:0000}", Math.Max(1, m_lineCount)), DefaultTextBrush).Width + 5;
        }

        /// <summary>
        /// Returns a formatted text object from a plain text string.
        /// </summary>
        private FormattedText GetFormattedText(string text, Brush brush)
        {
            FormattedText fmtText = new FormattedText(
                text,
                System.Globalization.CultureInfo.InvariantCulture,
                FlowDirection.LeftToRight,
                new Typeface(FontFamily, FontStyle, FontWeight, FontStretch),
                FontSize,
                brush);

            fmtText.Trimming = TextTrimming.None;
            fmtText.LineHeight = m_lineHeight;
            return fmtText;
        }

        private string GetVisibleLines()
        {
            // Find begin and end positions
            string t = Text;
            int lineStart = Math.Max(1, m_viewLineStart - m_lineRenderOffset);
            m_textRenderOffset = (lineStart - 1) * m_lineHeight;
            int lineEnd = Math.Min(m_lineCount + 1, m_viewLineEnd + m_lineRenderOffset);
            int line = 1;
            int posBegin = 0;
            int posEnd = t.Length;
            for(int i = 0; i < t.Length; ++i)
            {
                if (t[i] == '\n')
                {
                    ++line;
                    if (line == lineStart)
                    {
                        posBegin = Math.Min(i + 1, t.Length - 1);
                    }
                    else if (line == lineEnd)
                    {
                        posEnd = Math.Min(i + 1, t.Length - 1);
                        break;
                    }
                }
            }

            // Return visible substring, plus a fore and aft offset to improve syntax highlighting
            return t.Substring(posBegin, posEnd - posBegin);
        }

        #endregion

        #region Private Members

        // Rendering elements
        private VisualHost m_renderContainer;
        private VisualHost m_lineNumbersContainer;
        private ScrollViewer m_scrollViewer;

        // Text measurements
        private double m_lineHeight;
        private int m_lineCount;
        private int m_viewLineStart;
        private int m_viewLineEnd;
        private double m_textRenderOffset;

        // Line render offset
        private const int m_lineRenderOffset = 50;

        // Keyword and value matching hashsets
        private HashSet<string> m_keywords = new HashSet<string>
        {
            "import", "library", "is", "not", "and", "or", "null", "number", "integer", "boolean", "string", "external",
            "collection", "as", "increment", "decrement", "by", "if", "else", "begin", "end", "over", "until", "set", "coroutine",
            "loop", "from", "to", "while", "function", "return", "break", "type", "wait", "public", "private", "readonly"
        };
        private HashSet<string> m_values = new HashSet<string>
        {
            "true", "false", "null"
        };

        #endregion
    }

}
