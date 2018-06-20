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
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Linq;
using Tool.Net;
using Tool.CommView;
using Tool.XControl;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Threading;
using Tool.FileHandle;


namespace Tool.SelectAGV
{
    public enum WorkMode
    {
        SendFile,
        InitAGV
    }


    public partial class SelectAGVWin : Window
    {
        static public SelectAGVWin                            This       = null;
        RobotOnlineInfoCollect                                dcContent  = null;
        List<string>                                          SelectList = new List<string>();
        List<Tool.P2PConfig.TransFilesInfo>                    FileList   = new List<Tool.P2PConfig.TransFilesInfo>();
        WorkMode                                               Mode      = WorkMode.SendFile;
        string ToDir = null;
        public static NetInterface.fn_init_agv_t fn_init_agv_t_ = new NetInterface.fn_init_agv_t(
            (Int32 step,Int32 status,string ip) =>
            {
                This.Dispatcher.Invoke(() => {
                    WaitWindow.ChangeCallback(0, step);
                    if (status < 0)
                    {
                        string str = string.Format("{0}初始化失败", ip);
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, str, "错误");
                    }
                    return;
                });
            }
        );

        public SelectAGVWin(FrameworkElement Owner,List<RobotOnlineInfo> robotList,List<Tool.P2PConfig.TransFilesInfo> fileList, WorkMode mode,string toDir)
        {
            InitializeComponent();
            This = this;
            dcContent = RobotOnlineInfoCollect.GetInstance();
            ObservableCollection<RobotOnlineInfo> infocollection = new ObservableCollection<RobotOnlineInfo>(robotList);
            dcContent.Collection = infocollection;
            RobotOnlineList.DataContext = dcContent;
            RobotOnlineList.ItemsSource = dcContent.Collection.ToList();

            FileList = fileList;
            Mode = mode;
            ToDir = toDir;

            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnSelectAllRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<RobotOnlineInfo>  list= dcContent.Collection.ToList();
                SelectList = list.Select(l => l.Fts).ToList();
                dcContent.Collection.ToList().ForEach(p => p.IsSelected = true);
            }
            else
            {
                SelectList.Clear();
                dcContent.Collection.ToList().ForEach(p => p.IsSelected = false);
            }
        }

        private void OnSelectRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            string str = cb.Tag.ToString();
            if (true == cb.IsChecked)
            {
                SelectList.Add(str);
            }
            else
            {
                SelectList.Remove(str);
            }
        }

        private void OnClickSend(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
            switch (Mode)
            {
                case WorkMode.SendFile:
                    SendFile();
                    break;
                case WorkMode.InitAGV:
                    InitAGV();
                    break;
                default:
                    break;
            }
        }

        public void SendFile()
        {
            if (SelectList.Count <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要发送的IP！", "错误");
                return;
            }

            if (FileList.Count <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要发送的为文件！", "错误");
                return;
            }

            int retval = Tool.FileHandle.FTS.PushFiles(SelectList, FileList, ToDir);
            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "发送文件失败！", "错误");
                return;
            }

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "已经加入传输列表", 20, 5, Colors.Green);

            dcContent.Collection.ToList().ForEach(p => p.IsSelected = false);
            SelectList.Clear();

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
            
        }


        public void InitAGV()
        {
            if (SelectList.Count <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要发送的IP！", "错误");
                return;
            }

            string epList = null;
            foreach (string ep in SelectList)
            {
                if (ep == null) continue;
                epList += ep + "|";
            }

            int retval = -1;
            this.Dispatcher.Invoke(() =>
            {
                this.WaitEvent(() =>
                {
                    retval = NetInterface.NetInitFactorySetting(epList, fn_init_agv_t_);
                });
            });

            if(retval<0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "初始化失败", "错误");
                return;
            }

            dcContent.Collection.ToList().ForEach(p => p.IsSelected = false);
            SelectList.Clear();

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "初始化结束", 20, 5, Colors.Green);
            this.Dispatcher.Invoke(() => { Close(); });
        }

        private void OnClickCancle(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        public static void Show(FrameworkElement Owner, List<RobotOnlineInfo> robotList, List<Tool.P2PConfig.TransFilesInfo> fileList,WorkMode mode, string toDir)
        {
            SelectAGVWin wid = new SelectAGVWin(Owner, robotList, fileList, mode,toDir);
            wid.ShowDialog();
            return;
        }
    }

    public static class SelectAGVWindow
    {
        public static void Show(this FrameworkElement Owner, List<RobotOnlineInfo> robotList, List<Tool.P2PConfig.TransFilesInfo> fileList, WorkMode mode, string toDir)
        {
            SelectAGVWin.Show(Owner, robotList, fileList, mode,toDir);
        }
    }
}
