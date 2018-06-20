using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Windows.Threading;
using Tool.Net;
using Tool.CommView;


namespace Tool.TransferPanel
{
    public partial class TransFromDetailInfo : Window
    {
        public TransFromDetailInfo This=null;
        TaskFileDetailInfoCollect dcContent = null;
        
        List<string> SelectFileList = new List<string>();
        DispatcherTimer dispatcherTimer = new DispatcherTimer();
        UInt64 Taskid;
        public TransFromDetailInfo(FrameworkElement Owner, UInt64 taskid, List<TaskFileDetailInfo>List)
        {
            InitializeComponent();
            This = this;
            Taskid = taskid;

            Window pwin = Window.GetWindow(Owner);
            this.Owner = pwin;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            dcContent = TaskFileDetailInfoCollect.GetInstance();
            ObservableCollection<TaskFileDetailInfo> infocollection = new ObservableCollection<TaskFileDetailInfo>(List);
            dcContent.Collection = infocollection;
            FileDetailList.DataContext = dcContent;
            FileDetailList.ItemsSource = dcContent.Collection.ToList();

            dispatcherTimer.Tick += new EventHandler(DispatcherTimerTick);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            dispatcherTimer.Start();
        }

        private void DispatcherTimerTick(object sender, EventArgs e)
        {
            List<TaskFileDetailInfo> list = new List<TaskFileDetailInfo>();
            NetInterface.GetPushFilesDetailInfo(Taskid, ref list);
            ObservableCollection<TaskFileDetailInfo> infocollection = new ObservableCollection<TaskFileDetailInfo>(list);
            dcContent.Collection = infocollection;
            FileDetailList.DataContext = dcContent;
            FileDetailList.ItemsSource = dcContent.Collection.ToList();
        }

        private void DragLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                DragMove();
            }
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnSelectAllFiles(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<TaskFileDetailInfo> list = dcContent.Collection.ToList();
                SelectFileList = list.Select(l => l.FileName).ToList();
                dcContent.Collection.ToList().ForEach(p => p.IsSelectedFile = true);
            }
            else
            {
                SelectFileList.Clear();
                dcContent.Collection.ToList().ForEach(p => p.IsSelectedFile = false);
            }
        }

        private void OnSelectFiles(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            string str = cb.Tag.ToString();
            if (true == cb.IsChecked)
            {
                SelectFileList.Add(str);
            }
            else
            {
                SelectFileList.Remove(str);
            }
        }

        private void OnClickCancle(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnRepeatTransfrom(object sender, RoutedEventArgs e)
        {
            if (SelectFileList.Count <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要重传的文件！", "提示");
                return;
            }

            string dirList = null;
            foreach (string dir in SelectFileList)
            {
                if (dir == null) continue;
                dirList += dir + "|";
            }

            NetInterface.RepeatFilesToRemote(Taskid, dirList);
        }

        public static void Show(FrameworkElement Owner, UInt64 taskid,List<TaskFileDetailInfo> list)
        {
            TransFromDetailInfo wid = new TransFromDetailInfo(Owner, taskid,list);
            wid.ShowDialog();
            return;
        }
    }

    public static class TransFromDetailInfoWid
    {
        public static void Show(this FrameworkElement Owner, UInt64 taskid,List<TaskFileDetailInfo> list)
        {
            TransFromDetailInfo.Show(Owner, taskid,list);
        }
    }
}
