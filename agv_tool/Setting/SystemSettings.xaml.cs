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
using Tool.SelectAGV;
using Tool.Net;
using Tool.XControl;
using Tool.CommView;


namespace Tool.Setting
{
    public enum TransferBlockSize
    {
        block_4kb = 1,          //块大小
        block_8kb,
        block_16kb,
        block_32kb,
        block_64kb
    };

    public partial class SystemSettings : Window
    {
        List<RobotOnlineInfo> RobotList = new List<RobotOnlineInfo>();
        TransferBlockSize     BlockSize = TransferBlockSize.block_8kb;

        public SystemSettings(FrameworkElement Owner, List<RobotOnlineInfo> list, int BlockIndex)
        {
            InitializeComponent();
            Window pwin = Tool.MainWindow.This;
            this.Owner = pwin;
            Left = 0;
            Top = 0;
            Width = pwin.Width;
            Height = pwin.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            RobotList = list;
            InitVersioninfo();

            ComboxBlockSize.SelectedIndex = BlockIndex;
        }

        void InitVersioninfo()
        {
            string strVersion = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
            TBVersion.Text = string.Format("当前版本号：{0}", strVersion);

            string strLocalDir=null;
           
            string strPublishTime= System.IO.File.GetLastWriteTime(this.GetType().Assembly.Location).ToString();
            //Tool.Setting.ConfigRead.ReadConfig(ref strLocalDir,ref strPublishTime);

            TBPublishTime.Text = string.Format("发布时间：{0}", strPublishTime);

            TextLocal.Text = strLocalDir;

            string DHCP_IPv4 = null;
            string Port = null;
            Tool.Setting.ConfigRead.ReadDHCP(ref DHCP_IPv4, ref Port);
            DHCP_IP.Text = DHCP_IPv4;
            DHCP_Port.Text = Port;
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnInitAGV(object sender, RoutedEventArgs e)
        {
            SelectAGVWindow.Show(this.Owner, RobotList, null, WorkMode.InitAGV,null);
        }

        public static void Show(FrameworkElement Owner, List<RobotOnlineInfo> robotList,int blockIndex )
        {
            SystemSettings wid = new SystemSettings( Owner, robotList, blockIndex);
            wid.ShowDialog();
            return;
        }

        private void OnSelectLocalFolder(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog folder = new System.Windows.Forms.FolderBrowserDialog();
            folder.SelectedPath = TextLocal.Text;
            folder.ShowDialog();
            
            if (folder.SelectedPath == TextLocal.Text) return;

            int pos = folder.SelectedPath.IndexOf("\\");
            if (pos + 1 == folder.SelectedPath.Length)
            {
                TextLocal.Text = folder.SelectedPath + "standard";
            }
            else
            {
                TextLocal.Text = folder.SelectedPath + "\\standard";
            }
            Tool.Setting.ConfigRead.UpdataLocalDir(TextLocal.Text);
        }

        private void OnOpenLocalFloder(object sender, RoutedEventArgs e)
        {
            string path = TextLocal.Text.ToString();
            if (path == "") return;

            System.Diagnostics.Process.Start("explorer.exe ", path);
        }

        private void OnClickChangeBlockSize(object sender, RoutedEventArgs e)
        {
            NetInterface.NetSetBlockSize(BlockSize);
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "设置成功", 18, 5, Colors.Green);

            ((ComboBoxItem)(ComboxBlockSize.SelectedItem)).IsSelected = true;
            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnChangeBlockSize(object sender, SelectionChangedEventArgs e)
        {
            string str = "";
            if (ComboxBlockSize.Text != "")
            {
                str = ((ComboBoxItem)(ComboxBlockSize.SelectedItem)).Content.ToString();
            }

            if ("" == str) return;

            if (str == "4K")
            {
                BlockSize = TransferBlockSize.block_4kb;
                Tool.MainWindow.This.SetBlockIndex(0);
            }
            else if (str == "8K")
            {
                BlockSize = TransferBlockSize.block_8kb;
                Tool.MainWindow.This.SetBlockIndex(1);
            }
            else if (str == "16K")
            {
                BlockSize = TransferBlockSize.block_16kb;
                Tool.MainWindow.This.SetBlockIndex(2);
            }
            else if (str == "32K")
            {
                BlockSize = TransferBlockSize.block_32kb;
                Tool.MainWindow.This.SetBlockIndex(3);
            }
            else if (str == "64K")
            {
                BlockSize = TransferBlockSize.block_64kb;
                Tool.MainWindow.This.SetBlockIndex(4);
            }  
        }

        private void OnSaveDHCPAddr(object sender, RoutedEventArgs e)
        {

            if (Tool.Setting.ConfigRead.UpdataDHCP(DHCP_IP.Text.ToString(), DHCP_Port.Text.ToString()) < 0)
            {
                TipsMessageEx.ShowTips(Tool.MainWindow.This, "设置失败", 18, 5, Colors.Green);
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "设置成功", 18, 5, Colors.Green);

        }
    }

    public static class SystemSettingsWindow
    {
        public static void Show(this FrameworkElement Owner, List<RobotOnlineInfo> robotList, int blockIndex)
        {
            SystemSettings.Show(Owner, robotList, blockIndex);
        }
    }
}
