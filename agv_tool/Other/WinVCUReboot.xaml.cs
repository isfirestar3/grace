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
    public partial class WinVCUReboot : Window
    {
        int RobotID = -1;

        public WinVCUReboot(FrameworkElement Owner, int robotId,int enable)
        {

            InitializeComponent();

            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            if (0==enable)
            {
                RDYes.IsChecked = false;
                RDNo.IsChecked = true;
            }
            else
            {
                RDYes.IsChecked = true;
                RDNo.IsChecked = false;
            }
            RobotID = robotId;
        }

        private void OnClickSend(object sender, RoutedEventArgs e)
        {
            Tool.Proto.ProtoVCUEnable proto = new Proto.ProtoVCUEnable();
            int status = 0;
            if (true==RDYes.IsChecked)
            {
                status = 1;
            }
            else if(true == RDNo.IsChecked)
            {
                status = 0;
            }

            proto.VCUEnable.value_ = status;
            byte[] b_msg = new byte[proto.length()];
            Int32 offset = 0;
            proto.serialize(b_msg, ref offset);

            unsafe
            {
                fixed (byte* l_msg = b_msg)
                {
                    if (NetInterface.NetPostAsyncPackage(RobotID, (int)NetInterface.PKTTYPE.kAgvShellProto_VCUEnable, l_msg, proto.length()) < 0)
                    {
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "设置失败", "错误");
                        return;
                    }
                }
            }
           
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "设置成功", 18, 5, Colors.Green);
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
        public static void Show(FrameworkElement Owner, int robotId,int enable)
        {
            WinVCUReboot wid = new WinVCUReboot(Owner, robotId,enable);
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

    public static class WinVCURebootService
    {
        public static void Show(this FrameworkElement Owner, int robotId,int enable)
        {
            WinVCUReboot.Show(Owner, robotId,enable);
        }
    }
}
