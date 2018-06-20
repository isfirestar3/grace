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
using Tool.RobotsHome;
using System.Text.RegularExpressions;



namespace Tool.CarHouse
{
    public partial class CarEdit : Window
    {

        RobotInfo infoCur = null;
        public CarEdit(FrameworkElement owner, RobotInfo infoOld)
        {
            InitializeComponent();
            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            infoCur = infoOld;
           
            TBName.Text = infoCur.Id.ToString();
            var ep = infoCur.EndPoint.Split(':');
            if (ep.Length != 2) return;
            
            TBIPv4.Text = ep[0].ToString();
            TBPort.Text = ep[1].ToString();
        }

        private void CloseWid()
        {
            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
            CloseWid();
        }
        public static void Show(FrameworkElement Owner, RobotInfo infoPrev)
        {
            CarEdit wid = new CarEdit(Owner, infoPrev);
            wid.ShowDialog();
            return;
        }

        private bool CheckIP(string ep)
        {
            string strIp = ep;
            var array_ip = strIp.Split('.');


            if (array_ip.Length != 4)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP格式，例如192.168.1.1", "错误");
                return false;
            }

            uint ipv4_1 = 0;
            uint ipv4_2 = 0;
            uint ipv4_3 = 0;
            uint ipv4_4 = 0;

            try
            {
                ipv4_1 = UInt32.Parse(array_ip[0]);
                ipv4_2 = UInt32.Parse(array_ip[1]);
                ipv4_3 = UInt32.Parse(array_ip[2]);
                ipv4_4 = UInt32.Parse(array_ip[3]);
            }
            catch (Exception)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "输入正确的ip格式", "错误");
                return false;
            }
        
            

            if (ipv4_1>255|| ipv4_1 > 255|| ipv4_3 > 255|| ipv4_4 > 255)
            {
                return false;
            }

            return true;
        }

        private bool CheckArgs()
        {
            //IP或者域名校验
            if (TBIPv4.Text.ToString().Length <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "IP不能为空！", "错误");
                return false;
            }

            if(!CheckIP(TBIPv4.Text.ToString()))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP格式，例如192.168.255.255", "错误");
                return false;
            }

            //端口校验
            if (TBPort.Text.ToString().Length <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "端口不能为空！", "错误");
                return false;
            }

            int id;
            if (!isNumberic(TBPort.Text.ToString(), out id))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "端口必须为纯数字！", "错误");
                return false;
            }

            //编号校验
            if (TBName.Text.ToString().Length <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号不能为空！", "错误");
                return false;
            }

            if (!isNumberic(TBName.Text.ToString(), out id))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号必须为纯数字！", "错误");
                return false;
            }
            return true;
        }

        private void OnClickConfrim(object sender, RoutedEventArgs e)
        {
            //检查输入参数
            if (!CheckArgs())
            {
                CloseWid();
                return;
            }

            RobotInfo info = new RobotInfo();
            string str = TBIPv4.Text.ToString() + ":"+TBPort.Text.ToString();
            info.EndPoint = str;
           
            info.Id = Convert.ToInt32(TBName.Text.ToString());

            //更新XML文件
            if (!Tool.RobotsHome.RobotInfoCollect.GetInstance().UpdateRobotInfo(infoCur, info))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "修改失败请重试！", "错误");
                CloseWid();
                return;
            }

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "修改成功", 18, 5, Colors.Green);
            CloseWid();
        }

        private void OnClickCancle(object sender, RoutedEventArgs e)
        {
            CloseWid();
        }

        protected bool isNumberic(string message, out int result)
        {
            result = -1;
            try
            {
                result = Convert.ToInt32(message);
                return true;
            }
            catch
            {
                return false;
            }
        }

        private void OnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Regex re = new Regex("[^0-9.-]+");
            e.Handled = re.IsMatch(e.Text);
            if (e.Handled) return;
        }
    }

    public static class CarEditWin
    {
        public static void Show(this FrameworkElement Owner, RobotInfo infoPrev)
        {
            CarEdit.Show(Owner, infoPrev);
        }
    }
}
