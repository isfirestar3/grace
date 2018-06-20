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
using Tool.XControl;
using System.Text.RegularExpressions;

namespace Tool.ProcessConfig
{
    public partial class WinAppendProcess : Window
    {
        public WinAppendProcess(FrameworkElement Owner)
        {
            InitializeComponent();


            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
        }

        public static void Show(FrameworkElement Owner)
        {
            WinAppendProcess wid = new WinAppendProcess(Owner);
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

        private void OnClickAdd(object sender, RoutedEventArgs e)
        {
            string path = TBPath.Text.ToString();
            if (path.Length > 250)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "路径太长", "提示");
                return;
            }
            if (path.Trim() == string.Empty)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请输入软件路径", "提示");
                return;
            }


            string arg =  TBArg.Text.ToString();
            if (arg.Length > 250)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "启动参数太长", "提示");
                return;
            }


            string time = TBDeplyTime.Text.ToString().Trim();
            if (time.Length > 4)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "延迟时间太长，小于1000", "提示");
                return;
            }
            if (time.Trim() == string.Empty)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请输入延迟时间", "提示");
                return;
            }

            ProcessObjInfo info = new ProcessObjInfo();
            info.Name = "";
            info.Path = path;
            info.Arg = arg;
            info.DelayTime = Convert.ToInt32(time);

            if (!ProcessInfoCollect.GetInstance().AddProcess(info))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "添加失败！", "错误");
                return;
            }

            Tool.ProcessConfig.WinProcessDetail.This.Refresh();
            Tool.ProcessConfig.WinProcessDetail.This.UpdateProcessInfo();

            this.Dispatcher.Invoke(() => {Close(); });
        }

        private void OnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Regex re = new Regex("[^0-9.-]+");
            e.Handled = re.IsMatch(e.Text);
            if (e.Handled)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "输入的必须是数字", "错误");
                return;
            }

        }

        private void TBDeplyTimeExecuted(object sender, ExecutedRoutedEventArgs e)
        {

        }

        private void TBDeplyTimeCanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.Handled = true;
        }
    }
    public static class WinAddProcessService
    {
        public static void Show(this FrameworkElement Owner)
        {
            WinAppendProcess.Show(Owner);
        }
    }

}


