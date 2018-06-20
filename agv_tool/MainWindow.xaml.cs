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
using Tool.Net;
using Tool.CommView;
using Tool.XControl;
using Tool.TransferPanel;
using Tool.P2PConfig;
using Tool.SelectAGV;
using Tool.Setting;
using Tool.RobotsHome;
using Tool.UpdateBin;
using Tool.ProcessManager;

namespace Tool
{
    public partial class MainWindow : Window
    {
        public static MainWindow  This         = null;
        Rect                      NormalSize;
        int                       BlockSize    = 1; 
        public MainWindow()
        {
            InitializeComponent();
            This = this;
            Tool.Setting.ConfigRead.Read();
        }

        private void MainWindowsLoaded(object sender, RoutedEventArgs e)
        {
            WorkFrame.NavigationUIVisibility = NavigationUIVisibility.Hidden;
            WorkFrame.Navigate(new Tool.RobotsHome.RobotsHome());
        }

        private void OnClickMinWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
            WindowState = WindowState.Minimized;
        }

        private void OnClickMaxWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            if (Width == SystemParameters.WorkArea.Width &&
                Height == SystemParameters.WorkArea.Height)
            {
                Left = NormalSize.Left;
                Top = NormalSize.Top;
                Width = NormalSize.Width;
                Height = NormalSize.Height;
                Tool.RobotsHome.RobotsHome.This.RefrshRobotPageInfo(RobotInfoCollect.GetInstance().Collection.ToList(),10);
            }
            else
            {
                NormalSize = new Rect(this.Left, this.Top, this.Width, this.Height);
                Left = Top = 0;
                Width = SystemParameters.WorkArea.Width;
                Height = SystemParameters.WorkArea.Height;
                Tool.RobotsHome.RobotsHome.This.RefrshRobotPageInfo(RobotInfoCollect.GetInstance().Collection.ToList(),20);
            }
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            MessageBoxResult Result = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "确认是否退出？", "提示", MessageBoxButton.YesNo);
            if (Result == MessageBoxResult.No) return;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void DragLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                DragMove();
            }
        }

        public void NaviToPageConfig(string ep, Proto.ProtoFolderList list,int SelectIndex)
        {
            WorkFrame.Navigate(new Tool.P2PConfig.P2PConfig(ep,list, SelectIndex));
        }

        public void NaviToPagePreConfig()
        {
            WorkFrame.Navigate(Tool.P2PConfig.P2PConfig.This);

        }

        private void ToolBarMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount >= 2)
            {
                OnClickMaxWid(sender, e);
                e.Handled = true;
            }
        }

        private void OnClickTransfer(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            if (null == Tool.TransferPanel.FileTransferPage.This)
            {
                WorkFrame.Navigate(new Tool.TransferPanel.FileTransferPage());
            }
            else
            {
                WorkFrame.Navigate(Tool.TransferPanel.FileTransferPage.This);
            }

        }

        public void GotoHomeConfig()
        {
            WorkFrame.Navigate(Tool.RobotsHome.RobotsHome.This);
            Tool.RobotsHome.RobotsHome.This.SetCheck();
        }
        private void OnClickHomeConfig(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            GotoHomeConfig();
        }

        private void OnClickCarConfig(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            if (null == Tool.CarHouse.CartHousePage.This)
            {
                WorkFrame.Navigate(new Tool.CarHouse.CartHousePage());
            }
            else
            {
                WorkFrame.Navigate(Tool.CarHouse.CartHousePage.This);
                Tool.CarHouse.CartHousePage.This.SetCheck();
            }

        }

        private void OnClickSetting(object sender, RoutedEventArgs e)
        {
            List<RobotOnlineInfo> list = new List<RobotOnlineInfo>();
            RobotInfoCollect.GetInstance().FliterRobotOnline(ref list);
            SystemSettingsWindow.Show(This,list, BlockSize);
        }

        private void OnClickUpdateSoftware(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            if (null == Tool.UpdateBin.UpdateBin.This)
            {
                WorkFrame.Navigate(new Tool.UpdateBin.UpdateBin());
            }
            else
            {
                WorkFrame.Navigate(Tool.UpdateBin.UpdateBin.This);
                Tool.UpdateBin.UpdateBin.This.refresh();
            }
        }

        public void SetBlockIndex(int index)
        {
            BlockSize = index;
            Tool.Setting.ConfigRead.UpdataBlockSize(BlockSize);
        }

        private void OnClickManageProcess(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            if (null ==Tool.ProcessManager.ProcessManager.This)
            {
                WorkFrame.Navigate(new Tool.ProcessManager.ProcessManager());
            }
            else
            {
                WorkFrame.Navigate(Tool.ProcessManager.ProcessManager.This);
                Tool.ProcessManager.ProcessManager.This.RefreshTable();
                Tool.ProcessManager.ProcessManager.This.Refresh();
                
            }
        }

        private void OnClickConfigProcess(object sender, MouseButtonEventArgs e)
        {
            var itb = sender as XTabItemImage;
            if (null == itb) return;

            if (null == Tool.ProcessManager.ProcessManager.This)
            {
                WorkFrame.Navigate(new Tool.ProcessManager.ProcessManager());
            }
           

            if (null == Tool.ProcessConfig.ProcessConfig.This)
            {
                WorkFrame.Navigate(new Tool.ProcessConfig.ProcessConfig());
            }
            else
            {
                WorkFrame.Navigate(Tool.ProcessConfig.ProcessConfig.This);
                Tool.ProcessConfig.ProcessConfig.This.Refresh();
            }
        }
    }
}
