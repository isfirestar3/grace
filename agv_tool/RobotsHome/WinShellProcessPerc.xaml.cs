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
using Tool.CommView;

namespace Tool.RobotsHome
{
    /// <summary>
    /// Interaction logic for ShellProcessPerc.xaml
    /// </summary>
    public partial class WinShellProcessPerc : Window
    {
        public WinShellProcessPerc(FrameworkElement Owner)
        {
            InitializeComponent();
            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            if (RobotInfoCollect.GetInstance().Collection.Count>0)
            {
                this.ProcessPercTxt.DataContext = RobotInfoCollect.GetInstance().Collection[0];
                this.ProcessPercBar.DataContext = RobotInfoCollect.GetInstance().Collection[0];
                Tool.RobotsHome.RobotInfoCollect.GetInstance().UpdateShellPercent("", 0);
            }
           
        }
        public static void Show(FrameworkElement Owner)
        {
            WinShellProcessPerc wid = new WinShellProcessPerc(Owner);
            wid.ShowDialog();
            return;
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            MessageBoxResult Result = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "退出后无法查看进度,确认是否退出？", "提示", MessageBoxButton.YesNo);
            if (Result == MessageBoxResult.No) return;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }
    }

    public static class WinShellProcessPercService
    {
        public static void Show(this FrameworkElement Owner)
        {
            WinShellProcessPerc.Show(Owner);
        }
    }
}
