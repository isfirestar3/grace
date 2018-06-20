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
using Tool.XControl;


namespace Tool.TransferPanel
{
    public partial class FileTransferDetailInfo : Window
    {
        public FileTransferDetailInfo  This            =  null;
        TaskFileDetailInfoCollect      dcContent       =  null;
        List<string>                   SelectFileList  =  new List<string>();
        DispatcherTimer                DispatcherTimer =  new DispatcherTimer();
        UInt64                         Taskid;
        bool                           AllSelect       =  false;

        public FileTransferDetailInfo(FrameworkElement Owner, UInt64 taskid, List<TaskFileDetailInfo>List)
        {
            InitializeComponent();
            This = this;
            Taskid = taskid;

            Window pwin = Tool.MainWindow.This;
            this.Owner = pwin;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            dcContent = TaskFileDetailInfoCollect.GetInstance();
            ObservableCollection<TaskFileDetailInfo> infocollection = new ObservableCollection<TaskFileDetailInfo>(List);
            dcContent.Collection = infocollection;
            FileDetailList.DataContext = dcContent;
            FileDetailList.ItemsSource = dcContent.Collection.ToList();

            DispatcherTimer.Tick += new EventHandler(DispatcherTimerTick);
            DispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            DispatcherTimer.Start();
        }

        //定时器刷新状态
        private void DispatcherTimerTick(object sender, EventArgs e)
        {
            List<TaskFileDetailInfo> list = new List<TaskFileDetailInfo>();

            NetInterface.NetGetPushFilesDetailInfo(Taskid, ref list);
            Dictionary<string, TaskFileDetailInfo> tmpMap = list.ToDictionary(key => key.FileName, value => value);

            ObservableCollection<TaskFileDetailInfo> infocollection = new ObservableCollection<TaskFileDetailInfo>(list);
            dcContent.Collection = infocollection;
            FileDetailList.DataContext = dcContent;
            if (true == AllSelect)
            {
                dcContent.Collection.ToList().ForEach(p => p.IsFileSelected = true);
                FilesCheckBox.IsChecked = true;
                FileDetailList.ItemsSource = dcContent.Collection.ToList();
                return;
            }

            if (SelectFileList.Count > 0)
            {
                foreach (string filename in SelectFileList)
                {
                    if (tmpMap.ContainsKey(filename))
                    {
                        tmpMap[filename].IsFileSelected = true;
                    }
                }
                infocollection = new ObservableCollection<TaskFileDetailInfo>(tmpMap.Values.ToList());
                dcContent.Collection = infocollection;
                FileDetailList.DataContext = dcContent;
            }

            FileDetailList.ItemsSource = dcContent.Collection.ToList();

        }

        //窗口关闭
        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        //单选多选
        private void OnSelectAllFiles(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<TaskFileDetailInfo> list = dcContent.Collection.ToList();
                SelectFileList = list.Select(l => l.FileName).ToList();
                dcContent.Collection.ToList().ForEach(p => p.IsFileSelected = true);
                AllSelect = true;
            }
            else
            {
                SelectFileList.Clear();
                dcContent.Collection.ToList().ForEach(p => p.IsFileSelected = false);
                AllSelect = false;
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


        //文件重传
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

            int retval = NetInterface.NetRepeatFilesToRemote(Taskid, dirList, dirList.Length);
            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "重传失败", "错误");
                return;
            }

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "任务继续，请稍后。。。", 18, 5, Colors.Green);
            dcContent.Collection.ToList().ForEach(p => p.IsFileSelected = false);
            SelectFileList.Clear();
            FilesCheckBox.IsChecked = false;
        }

        public static void Show(FrameworkElement Owner, UInt64 taskid,List<TaskFileDetailInfo> list)
        {
            FileTransferDetailInfo wid = new FileTransferDetailInfo(Owner, taskid,list);
            wid.ShowDialog();
            return;
        }
    }

    public static class TransFromDetailInfoWid
    {
        public static void Show(this FrameworkElement Owner, UInt64 taskid,List<TaskFileDetailInfo> list)
        {
            FileTransferDetailInfo.Show(Owner, taskid,list);
        }
    }
}
