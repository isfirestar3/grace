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
using System.Threading;
using Tool.CommView;
using Tool.XControl;
using Tool.RobotsHome;
using System.Text.RegularExpressions;

namespace Tool.CarHouse
{
    public partial class CarProduce : Window
    {
        List<RobotInfo> listProduce = new List<RobotInfo>();

        public CarProduce(FrameworkElement Owner)
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
            CarProduce wid = new CarProduce(Owner);
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

        private bool CheckArgs()
        {
            string strId = TBAddID.Text.ToString();
            int id;
            if (!isNumberic(strId, out id))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号必须为纯数字！", "错误");
                return false;
            }

            string strIp = TBIP.Text.ToString();
            var array_ip = strIp.Split('.');

            if (array_ip.Length != 4)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP格式，例如192.168.1.1", "错误");
                return false;
            }

            if (!CheckIP(strIp))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP格式，例如192.168.1.1", "错误");
                return false;
            }


            string strPort = TBPort.Text.ToString();
            int port;
            if (!isNumberic(strPort, out port))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号必须为纯数字！", "错误");
                return false;
            }

            if (port > 65536 || port < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "端口范围：0-65536！", "错误");
                return false;
            }
            return true;
        }

        private bool CheckIP(string ep)
        {
            string strIp = ep;
            var array_ip = strIp.Split('.');

            if (array_ip.Length != 4)
            {
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



            if (ipv4_1 > 255 || ipv4_1 > 255 || ipv4_3 > 255 || ipv4_4 > 255)
            {
                return false;
            }

            return true;
        }

        private void OnClickConfrim(object sender, RoutedEventArgs e)
        {
            //参数检查
            if (!CheckArgs()) return;
            
            //格式化数据
            RobotInfo info = new RobotInfo();
            info.Id =Convert.ToInt32( TBAddID.Text.ToString());
            info.EndPoint = TBIP.Text.ToString() + ":"+ TBPort.Text.ToString();
            info.NetId = 0;
            info.NetState = false;
            info.Fts = TBIP.Text.ToString() + ":"+"4411";

            List<RobotInfo> list = new List<RobotInfo>();
            list.Add(info);

            //接口调用写入XML
            if (!RobotInfoCollect.GetInstance().AppendRobotInfo(list))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "添加信息失败", "提示");
                return;
            }

            //刷新界面
            Tool.RobotsHome.RobotsHome.This.Refresh();
            Tool.CarHouse.CartHousePage.This.Refresh();

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "添加信息成功", 18, 5, Colors.Green);
            CloseWid();
        }

        private void CloseWid()
        {
            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
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

        private void OnClickBatchConfrim(object sender, RoutedEventArgs e)
        {
            if (0==listProduce.Count)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "预览列表为空！", "提示");
                return;
            }

            //接口调用写入XML
            if (!RobotInfoCollect.GetInstance().AppendRobotInfo(listProduce))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "批量添加信息失败", "提示");
                return;
            }

            //刷新界面
            Tool.RobotsHome.RobotsHome.This.Refresh();
            Tool.CarHouse.CartHousePage.This.Refresh();

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "批量添加信息成功", 18, 5, Colors.Green);
            CloseWid();
        }

        private void OnClickBatchCancle(object sender, RoutedEventArgs e)
        {
            CloseWid();
        }

        private void BatchProduce(int num, int id, string strPort, uint ipv4_1, uint ipv4_2, uint ipv4_3, uint ipv4_4)
        {
            listProduce.Clear();
            this.WaitEvent(() =>
            {
                for (uint i = 0; i < num; i++)
                {
                    RobotInfo info = new RobotInfo();
                    info.Id = (int)(i + 1);
                    Convert.ToInt32((id + i).ToString());
                    info.Id = Convert.ToInt32((id + i).ToString());

                    if (ipv4_4 + 1 < 255)
                    {
                        ipv4_4++;
                    }
                    else if (ipv4_3 + 1 < 255)
                    {
                        ipv4_3++;
                        ipv4_4 = 0;
                    }
                    else if (ipv4_2 + 1 < 255)
                    {
                        ipv4_2++;
                        ipv4_3 = 0;
                        ipv4_4 = 0;
                    }
                    else if (ipv4_1 + 1 < 255)
                    {
                        ipv4_1++;
                        ipv4_2 = 0;
                        ipv4_3 = 0;
                        ipv4_4 = 0;
                    }

                    string strStreamIP = string.Format("{0}.{1}.{2}.{3}", ipv4_1, ipv4_2, ipv4_3, ipv4_4);
                    info.EndPoint = strStreamIP + ":" + strPort;
                    info.NetId = 0;
                    info.NetState = false;
                    info.Fts = strStreamIP + ":" + "4411";
                    listProduce.Add(info);
                    Thread.Sleep(1);
                    WaitWindow.ChangeCallback(0, (int)((i + 1) * 100 / num));
                }
            });

            PreviewRobotList.ItemsSource = listProduce;
        }


        private bool CheckBatchArgs()
        {
            string strId = TBBatchAddID.Text.ToString();
            int id;
            if (!isNumberic(strId, out id))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号必须为纯数字", "错误");
                return false;
            }

            string strIp = TBBatchIP.Text.ToString();
            var array_ip = strIp.Split('.');


            if (array_ip.Length != 4)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP格式，例如192.168.1.1", "错误");
                return false;
            }

            uint ipv4_1 =0;
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
            catch(Exception)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "输入正确的ip格式", "错误");
                return false;
            }


            if (ipv4_1 > 255 || ipv4_2 > 255 || ipv4_3 > 255 || ipv4_4 > 255)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请正确的IP范围", "错误");
                return false;
            }

            string strPort = TBBatchPort.Text.ToString();
            int port;
            if (!isNumberic(strPort, out port))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "编号必须为纯数字", "错误");
                return false;
            }

            if (port > 65536 || port < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "端口范围：0-65536", "错误");
                return false;
            }

            string strNum = TBBatchNum.Text.ToString();

            if (strNum.Length > 4)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "输入的数量太大(<999）。", "错误");
                return false;
            }
            int num;
            if (!isNumberic(strNum, out num))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "批量生成必须为纯数字", "错误");
                return false;
            }
            return true;
        }

        private void OnClickPreview(object sender, RoutedEventArgs e)
        {
            if (!CheckBatchArgs()) return;

            string strIp = TBBatchIP.Text.ToString();
            var array_ip = strIp.Split('.');

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
                return;
            }

            BatchProduce(Convert.ToInt32(TBBatchNum.Text), Convert.ToInt32(TBBatchAddID.Text), TBBatchPort.Text.ToString(), ipv4_1, ipv4_2, ipv4_3, --ipv4_4);
            PreviewRobotList.Items.Refresh();
        }

        private void OnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Regex re = new Regex("[^0-9-]+");
            e.Handled = re.IsMatch(e.Text);
            if (e.Handled) return;
        }
    }

    public static class CarProduceWin
    {
        public static void Show(this FrameworkElement Owner)
        {
            CarProduce.Show(Owner);
        }
    }
}
