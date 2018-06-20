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


namespace Tool.Other
{
    public partial class WinNTP : Window
    {
        string RobotList;
        public WinNTP(FrameworkElement Owner, string  robotList)
        {
            InitializeComponent();
            RobotList = robotList;

            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
        }

        private void OnClickSend(object sender, RoutedEventArgs e)
        {
            string ip = TBIP.Text.ToString();
            if (ip == String.Empty)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请输入IP", "提示");
                return;
            }

            Tool.Proto.ProtoMSG protomsg = new Proto.ProtoMSG();
            protomsg.MSG.value_ = ip;
            byte[] b_msg = new byte[protomsg.length()];
            Int32 offset = 0;
            protomsg.serialize(b_msg, ref offset);

            var array_ip = RobotList.Split('|');
            foreach (string robot_id in array_ip)
            {
                if (robot_id == String.Empty) continue;
                unsafe
                {
                    fixed (byte* l_msg = b_msg)
                    {
                        NetInterface.NetPostAsyncPackage(Convert.ToInt32(robot_id), (int)NetInterface.PKTTYPE.kAgvShellProto_UpdateNTPServer, l_msg, protomsg.length());
                    }
                }
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "设置NTP服务成功", 18, 5, Colors.Green);
            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnClickCancle(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }
        public static void Show(FrameworkElement Owner, string robotList)
        {
            WinNTP wid = new WinNTP(Owner, robotList);
            wid.ShowDialog();
            return;
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }
    }

    public static class WinNTPService
    {
        public static void Show(this FrameworkElement Owner, string robotList)
        {
            WinNTP.Show(Owner, robotList);
        }
    }
}
