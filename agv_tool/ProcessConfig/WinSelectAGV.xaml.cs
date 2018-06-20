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
using Tool.Net;
using Tool.CommView;
using Tool.XControl;
using Tool.SelectAGV;
using System.Collections.ObjectModel;
using Tool.RobotsHome;

namespace Tool.ProcessConfig
{
    public partial class WinSelectAGV : Window
    {
        static public WinSelectAGV This = null;
        RobotOnlineInfoCollect dcContent = null;
        List<string> SelectList = new List<string>();
        List<ProcessObjInfo> ProcessList = new List<ProcessObjInfo>();
        public WinSelectAGV(FrameworkElement Owner, List<ProcessObjInfo> objList)
        {
            InitializeComponent();

            List<RobotOnlineInfo> list = new List<RobotOnlineInfo>();
            RobotInfoCollect.GetInstance().FliterRobotOnline(ref list);
            dcContent = RobotOnlineInfoCollect.GetInstance();
            ObservableCollection<RobotOnlineInfo> infocollection = new ObservableCollection<RobotOnlineInfo>(list);
            dcContent.Collection = infocollection;
            RobotOnlineList.DataContext = dcContent;
            RobotOnlineList.ItemsSource = dcContent.Collection.ToList();

            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            ProcessList = objList;
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
                List<RobotOnlineInfo> list = dcContent.Collection.ToList();
                SelectList = list.Select(l => l.EP).ToList();
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
            if (0 == ProcessList.Count) return;

            Tool.Proto.ProtoProcessDetail proto = new Proto.ProtoProcessDetail();
            foreach (var obj in ProcessList)
            {
                Tool.Proto.ProcessDetail info = new Proto.ProcessDetail();
                info.ProcessName.value_ = obj.Name;
                info.ProcessDir.value_ = obj.Path;
                info.ProcessArg.value_ = obj.Arg;
                info.ProcessDelayTime.value_ = obj.DelayTime;
                proto.List.Add(info);
            }

            byte[] b_msg = new byte[proto.length()];
            Int32 offset = 0;
            if (!proto.serialize(b_msg, ref offset)) return;
            int retval = -1;

            this.WaitEvent(() =>
            {
                foreach(string ep in SelectList)
                {
                    unsafe
                    {
                        fixed (byte* l_msg = b_msg)
                        {
                            Int32 netid = RobotInfoCollect.GetInstance().FindNetId(ep);
                            if (netid < 0) continue;
                            void* data = null;
                            int len = 0;
                            retval = NetInterface.NetPostSyncPackage(netid, (int)NetInterface.PKTTYPE.kAgvShellProto_UpdateProcessDetail, l_msg, proto.length(), &data, &len);
                            NetInterface.FreeDetail(data);
                        }
                    }
                }
            }, true, true);

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "已发送更新软件配置请求，详情请查看配置页面", 18, 5, Colors.Green);
        }


        private void OnClickCancle(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        public static void Show(FrameworkElement Owner,List<ProcessObjInfo> list)
        {
            WinSelectAGV wid = new WinSelectAGV(Owner, list);
            wid.ShowDialog();
            return;
        }
    }

    public static class WindowSelectAGVService
    {
        public static void Show(this FrameworkElement Owner,List<ProcessObjInfo> list)
        {
            WinSelectAGV.Show(Owner, list);
        }
    }
}
