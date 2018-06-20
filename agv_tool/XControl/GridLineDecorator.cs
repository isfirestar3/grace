using System;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Markup;
using System.Windows.Media;

namespace Tool.XControl
{
    internal static class GridViewColumnHelper
    {
        private static PropertyInfo DesiredWidthProperty =
            typeof(GridViewColumn).GetProperty("DesiredWidth", BindingFlags.NonPublic | BindingFlags.Instance);

        public static double GetColumnWidth(this GridViewColumn column)
        {
            return (double.IsNaN(column.Width)) ? (double)DesiredWidthProperty.GetValue(column, null) : column.Width;
        }
    }

    public static class VisualService
    {
        public static double GetDpiFactor(this Visual target)
        {
            var source = PresentationSource.FromVisual(target);
            return source == null ? 1.0 : 1 / source.CompositionTarget.TransformToDevice.M11;
        }

        public static T GetAncestor<T>(this DependencyObject target)
            where T : DependencyObject
        {
            var parent = VisualTreeHelper.GetParent(target);

            if (parent is T) return (T)parent;
            if (parent != null) return parent.GetAncestor<T>();

            return null;
        }

        public static T GetDesendentChild<T>(this DependencyObject target)
            where T : DependencyObject
        {
            var childCount = VisualTreeHelper.GetChildrenCount(target);
            if (childCount == 0) return null;

            for (int i = 0; i < childCount; i++)
            {
                var current = VisualTreeHelper.GetChild(target, i);
                if (current is T) return (T)current;

                var desendent = current.GetDesendentChild<T>();
                if (desendent != null) return desendent;
            }
            return null;
        }
    }

    [ContentProperty("Target")]
    public class GridLineDecorator : FrameworkElement
    {
        private ListView _target;
        private DrawingVisual _gridLinesVisual = new DrawingVisual();
        private GridViewHeaderRowPresenter _headerRowPresenter = null;
        private double _viewWidth = double.NaN;
        private bool _leftHorizontalLine = true;
        private bool _HorizontalLine = true;

        public GridLineDecorator()
        {
            this.AddVisualChild(_gridLinesVisual);
            this.AddHandler(ScrollViewer.ScrollChangedEvent, new RoutedEventHandler(OnScrollChanged));
        }

        #region LeftHorizontalLine

        public static readonly DependencyProperty LeftHorizontalLineProperty =
            DependencyProperty.Register("LeftHorizontalLine", typeof(bool), typeof(GridLineDecorator),
                new FrameworkPropertyMetadata(true,
                    new PropertyChangedCallback(OnLeftHorizontalLineChanged)));

        public bool LeftHorizontalLine
        {
            get { return (bool)GetValue(LeftHorizontalLineProperty); }
            set { SetValue(LeftHorizontalLineProperty, value); }
        }

        private static void OnLeftHorizontalLineChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ((GridLineDecorator)d).OnLeftHorizontalLineChanged(e);
        }

        protected virtual void OnLeftHorizontalLineChanged(DependencyPropertyChangedEventArgs e)
        {
            _leftHorizontalLine = (bool)e.NewValue;
            DrawGridLines();
        }


        #endregion

        #region HorizontalLine

        public static readonly DependencyProperty HorizontalLineProperty =
            DependencyProperty.Register("HorizontalLine", typeof(bool), typeof(GridLineDecorator),
                new FrameworkPropertyMetadata(true,
                    new PropertyChangedCallback(OnHorizontalLineChanged)));

       
        public bool HorizontalLine
        {
            get { return (bool)GetValue(LeftHorizontalLineProperty); }
            set { SetValue(LeftHorizontalLineProperty, value); }
        }

        
        private static void OnHorizontalLineChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ((GridLineDecorator)d).OnHorizontalLineChanged(e);
        }

      
        protected virtual void OnHorizontalLineChanged(DependencyPropertyChangedEventArgs e)
        {
            _HorizontalLine = (bool)e.NewValue;
            DrawGridLines();
        }


        #endregion

        #region GridLineBrush

       
        public static readonly DependencyProperty GridLineBrushProperty =
            DependencyProperty.Register("GridLineBrush", typeof(Brush), typeof(GridLineDecorator),
                new FrameworkPropertyMetadata(Brushes.LightGray,
                    new PropertyChangedCallback(OnGridLineBrushChanged)));

      
        public Brush GridLineBrush
        {
            get { return (Brush)GetValue(GridLineBrushProperty); }
            set { SetValue(GridLineBrushProperty, value); }
        }

        
        private static void OnGridLineBrushChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ((GridLineDecorator)d).OnGridLineBrushChanged(e);
        }

        protected virtual void OnGridLineBrushChanged(DependencyPropertyChangedEventArgs e)
        {
            DrawGridLines();
        }

        #endregion

        #region Target

        public ListView Target
        {
            get { return _target; }
            set
            {
                if (_target != value)
                {
                    if (_target != null) Detach();
                    RemoveVisualChild(_target);
                    RemoveLogicalChild(_target);

                    _target = value;

                    AddVisualChild(_target);
                    AddLogicalChild(_target);
                    if (_target != null) Attach();

                    InvalidateMeasure();
                }
            }
        }
        private void GetGridViewHeaderPresenter()
        {
            if (Target == null)
            {
                _headerRowPresenter = null;
                return;
            }
            _headerRowPresenter = Target.GetDesendentChild<GridViewHeaderRowPresenter>();
        }

        #endregion

        #region DrawGridLines

        private void DrawGridLines()
        {
            if (Target == null) return;

            var drawingContext = _gridLinesVisual.RenderOpen();
            var itemCount = Target.Items.Count;
            if (itemCount == 0)
            {
                drawingContext.Close();
                return;
            }

            var gridView = Target.View as GridView;

            // 获取drawingContext

            var startPoint = new Point(0, 0);
            var hLineY = 0.0;

            // 为了对齐到像素的计算参数，否则就会看到有些线是模糊的
            var dpiFactor = this.GetDpiFactor();
            var pen = new Pen(this.GridLineBrush, 1 * dpiFactor);
            var halfPenWidth = pen.Thickness/2 ;
            var guidelines = new GuidelineSet();

            // 画横线
            var lvRender = Target.RenderSize;
            var offsetS = Target.TranslatePoint(startPoint, this);
            var hLineX1 = offsetS.X + Target.BorderThickness.Left;
            var hLineX2 = offsetS.X + _viewWidth;

            bool firstLine = true;
            for (int i = 0; i < itemCount; i++)
            {
                var item = Target.ItemContainerGenerator.ContainerFromIndex(i) as ListViewItem;
                if (item != null)
                {
                    var renderSize = item.RenderSize;
                    var offset = item.TranslatePoint(startPoint, this);
                    if (offset.Y < offsetS.Y)
                    {
                        continue;
                    }

                    hLineY = offset.Y;
                    if (firstLine)
                    {
                        firstLine = false;
                        // 加入参考线，对齐到像素
                        guidelines.GuidelinesY.Add(hLineY + halfPenWidth);
                        drawingContext.PushGuidelineSet(guidelines);
                        drawingContext.DrawLine(pen, new Point(hLineX1, hLineY), new Point(hLineX2, hLineY));
                        drawingContext.Pop();
                    }
                    hLineY += renderSize.Height;
                    if (hLineY > offsetS.Y + Target.ActualHeight)
                    {
                        continue;
                    }

                    // 加入参考线，对齐到像素
                    guidelines.GuidelinesY.Add(hLineY + halfPenWidth);
                    drawingContext.PushGuidelineSet(guidelines);
                    drawingContext.DrawLine(pen, new Point(hLineX1, hLineY), new Point(hLineX2, hLineY));
                    drawingContext.Pop();
                }
            }

            // 画竖线
            if (null == gridView || null == _headerRowPresenter || !_HorizontalLine)
            {
                drawingContext.Close();
                return;
            }
            var columns = gridView.Columns;
            var headerOffset = _headerRowPresenter.TranslatePoint(startPoint, this);
            var headerSize = _headerRowPresenter.RenderSize;

            var vLineX = headerOffset.X;
            var vLineY1 = headerOffset.Y + headerSize.Height;

            if (hLineY > offsetS.Y + Target.ActualHeight)
            {
                hLineY = offsetS.Y + Target.ActualHeight;
            }

            firstLine = _leftHorizontalLine;
            foreach (var column in columns)
            {
                var columnWidth = column.GetColumnWidth();
                if (firstLine)
                {
                    firstLine = false;
                    if (offsetS.X >= headerOffset.X + _viewWidth)
                    {
                        continue;
                    }
                    // 加入参考线，对齐到像素
                    guidelines.GuidelinesX.Add(offsetS.X + halfPenWidth);
                    drawingContext.PushGuidelineSet(guidelines);
                    drawingContext.DrawLine(pen, new Point(offsetS.X, vLineY1), new Point(offsetS.X, hLineY));
                    drawingContext.Pop();
                }
                vLineX += columnWidth;
                if (vLineX >= headerOffset.X + _viewWidth)
                {
                    continue;
                }

                // 加入参考线，对齐到像素
                guidelines.GuidelinesX.Add(vLineX + halfPenWidth);
                drawingContext.PushGuidelineSet(guidelines);
                drawingContext.DrawLine(pen, new Point(vLineX, vLineY1), new Point(vLineX, hLineY));
                drawingContext.Pop();
            }
            drawingContext.Close();
        }

        #endregion

        #region Overrides to show Target and grid lines

        protected override int VisualChildrenCount
        {
            get { return Target == null ? 1 : 2; }
        }

        protected override System.Collections.IEnumerator LogicalChildren
        {
            get { yield return Target; }
        }

        protected override Visual GetVisualChild(int index)
        {
            if (index == 0) return _target;
            if (index == 1) return _gridLinesVisual;
            throw new IndexOutOfRangeException(string.Format("Index of visual child '{0}' is out of range", index));
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            if (Target != null)
            {
                Target.Measure(availableSize);
                return Target.DesiredSize;
            }
            return base.MeasureOverride(availableSize);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            if (Target != null)
            {
                Target.Arrange(new Rect(new Point(0, 0), finalSize));
            }
            return base.ArrangeOverride(finalSize);
        }

        #endregion

        #region Handle Events

        private void Attach()
        {
            _target.Loaded += OnTargetLoaded;
            _target.Unloaded += OnTargetUnloaded;
        }

        private void Detach()
        {
            _target.Loaded -= OnTargetLoaded;
            _target.Unloaded -= OnTargetUnloaded;
        }

        private void OnTargetLoaded(object sender, RoutedEventArgs e)
        {
            var gridView = _target.View as GridView;
            if (null != gridView)
            {
                gridView.Columns.CollectionChanged += OnGridViewCollectionChanged;
                
            }

            if (_headerRowPresenter == null)
            {
                GetGridViewHeaderPresenter();
            }
            DrawGridLines();
        }

        private void OnTargetUnloaded(object sender, RoutedEventArgs e)
        {
            var gridView = _target.View as GridView;
            if (null != gridView)
            {
                gridView.Columns.CollectionChanged -= OnGridViewCollectionChanged;
            }

            if (_headerRowPresenter == null)
            {
                GetGridViewHeaderPresenter();
            }
            DrawGridLines();
        }

        private void OnGridViewCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {

            if (_headerRowPresenter == null)
            {
                GetGridViewHeaderPresenter();
            }
            DrawGridLines();
        }

        private void OnScrollChanged(object sender, RoutedEventArgs e)
        {

            ScrollViewer sv = e.OriginalSource as ScrollViewer;
            _viewWidth = sv.ViewportWidth;

            if (_headerRowPresenter == null)
            {
                GetGridViewHeaderPresenter();
            }
            DrawGridLines();
        }

        #endregion
    }
}
