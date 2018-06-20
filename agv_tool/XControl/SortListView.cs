using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Linq;
using System.Collections.Generic;

namespace Tool.XControl
{
    public enum ListSortDirectionEx
    {
        //
        // 摘要:
        //     按升序排序。
        Ascending = 1,
        //
        // 摘要:
        //     按降序排序。
        Descending = 2,
        //
        // 摘要:
        //     已存在按相同的排序方式，否则升序排序
        AscendingExistSame = 3,
        //
        // 摘要:
        //     已存在按相同的排序方式，否则降序排序
        DescendingExistSame = 4,
        //
        // 摘要:
        //     已存在按相反的排序方式，否则升序排序
        AscendingExistContraries = 5,
        //
        // 摘要:
        //     已存在按相反的排序方式，否则降序排序
        DescendingExistContraries = 6
    }

    public class SortListView : ListView
    {
        public Func<string, Dictionary<string, ListSortDirectionEx>> TransformNameToDirection { private get; set; }

        public SortListView(): base()
        {

            TransformNameToDirection = (name) => {
                Dictionary<string, ListSortDirectionEx> sortTable = new Dictionary<string, ListSortDirectionEx>();
                sortTable.Add(name, ListSortDirectionEx.Ascending);
                return sortTable;
            };
            AddHandler(ScrollViewer.ScrollChangedEvent, new RoutedEventHandler(OnScrollChanged));
            AddHandler(System.Windows.Controls.Primitives.ButtonBase.ClickEvent, new System.Windows.RoutedEventHandler((sender, e) => {
                GridViewColumnHeader gch = e.OriginalSource as GridViewColumnHeader;
                try
                {
                    SortList(TransformNameToDirection(gch.Column.Header.ToString()));
                }
                catch (Exception)
                {

                }
            }));
            AddHandler(KeyDownEvent, new KeyEventHandler(OnKeyDown));
        }

        private double view_length_;

        public double ViewLength
        {
            get
            {
                return (double)GetValue(ViewLengthProperty);
            }
            set
            {
                SetValue(ViewLengthProperty, value);
            }
        }

        public static readonly DependencyProperty ViewLengthProperty =
            DependencyProperty.Register("ViewLength", typeof(double),
                typeof(SortListView),new PropertyMetadata(double.NaN, new PropertyChangedCallback(OnViewLengthChanged)));

        static void OnViewLengthChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            SortListView source = (SortListView)sender;
            source.view_length_ = (double)args.NewValue;
        }

        private void OnScrollChanged(object sender, RoutedEventArgs e)
        {
            ScrollViewer sv = e.OriginalSource as ScrollViewer;
            ViewLength = sv.ViewportWidth;
        }

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            switch (e.Key)
            {
                case Key.Enter:
                    e.Handled = true;
                    break;
                default:
                    break;
            }
            if (e.Handled)
            {
                var mbea = new MouseButtonEventArgs(Mouse.PrimaryDevice, 1, MouseButton.Left);
                mbea.RoutedEvent = ListView.MouseDoubleClickEvent;
                this.OnMouseDoubleClick(mbea);
            }
        }

        private void SortList(Dictionary<string, ListSortDirectionEx> sortTable)
        {

            ICollectionView dataView = CollectionViewSource.GetDefaultView(this.ItemsSource);//获取数据源视图
            foreach (var sortDec in dataView.SortDescriptions)
            {
                var selected_ret = sortTable.Where(sortItem => sortItem.Key == sortDec.PropertyName).Select(sortItem => sortItem);
                if (selected_ret.Count() <= 0)
                {
                    continue;
                }
                switch (selected_ret.First().Value)
                {
                    case ListSortDirectionEx.AscendingExistSame:
                    case ListSortDirectionEx.DescendingExistSame:
                        sortTable[selected_ret.First().Key] = (ListSortDirectionEx)sortDec.Direction;
                        break;
                    case ListSortDirectionEx.AscendingExistContraries:
                    case ListSortDirectionEx.DescendingExistContraries:
                        sortTable[selected_ret.First().Key] = sortDec.Direction == ListSortDirection.Ascending ? ListSortDirectionEx.Descending : ListSortDirectionEx.Ascending;
                        break;
                }
            }
            dataView.SortDescriptions.Clear();

            foreach (var sortItem in sortTable)
            {
                ListSortDirection direction = ListSortDirection.Descending;
                switch (sortItem.Value)
                {
                    case ListSortDirectionEx.Ascending:
                    case ListSortDirectionEx.AscendingExistContraries:
                    case ListSortDirectionEx.AscendingExistSame:
                        direction = ListSortDirection.Ascending;
                        break;
                }
                SortDescription sd = new SortDescription(sortItem.Key, direction);
                dataView.SortDescriptions.Add(sd);//加入新的排序描述
            }
            dataView.Refresh();//刷新视图
        }
    }
}
