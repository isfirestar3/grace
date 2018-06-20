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
using Tool.Net;
using Tool.XControl;



namespace Tool.ProcessConfig
{
    public partial class WinProcessDetail : Window
    {
        List<ProcessObjInfo> List = new List<ProcessObjInfo>();
        ProcessInfoCollect dcProcessInfo = null;
        Int32 RobotId = 0;
        public static WinProcessDetail This = null;
        public WinProcessDetail(FrameworkElement Owner, Proto.ProtoProcessDetail list,string net_id)
        {
            InitializeComponent();
            RobotId = Convert.ToInt32(net_id);
            This = this;

            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            int i = 0;
            foreach(var info in list.List)
            {

                ProcessObjInfo obj = new ProcessObjInfo();
                obj.ID = ++i;
                obj.Name = info.ProcessName.value_;
                obj.Path = info.ProcessDir.value_;
                obj.Arg = info.ProcessArg.value_;
                obj.DelayTime = info.ProcessDelayTime.value_;
                List.Add(obj);
            }
            ProcessInfoCollect.GetInstance().AddProcess(List);
            Refresh();
        }

        public void Refresh()
        {
            
            dcProcessInfo = ProcessInfoCollect.GetInstance();
            RobotList.DataContext = dcProcessInfo;
            RobotList.ItemsSource = dcProcessInfo.Collection.ToList();
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }
        public static void Show(FrameworkElement Owner, Proto.ProtoProcessDetail list,string net_id)
        {
            WinProcessDetail wid = new WinProcessDetail(Owner,list, net_id);
            wid.ShowDialog();
            return;
        }

        private void OnDeleteProcess(object sender, RoutedEventArgs e)
        {
            Tool.XControl.XButton Btn = sender as Tool.XControl.XButton;
            string id = Btn.Tag.ToString();

            MessageBoxResult Result = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "确定要删除此进程？", "提示", MessageBoxButton.YesNo);
            if (Result == MessageBoxResult.No) return;


            if (!ProcessInfoCollect.GetInstance().RemoveProcess(Convert.ToInt32(id)))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "删除失败！", "错误");
                return;
            }

            if (UpdateProcessInfo() < 0) return;
            Refresh();
        }

        private void OnDownProcess(object sender, RoutedEventArgs e)
        {
            Tool.XControl.XButton Btn = sender as Tool.XControl.XButton;
            string id = Btn.Tag.ToString();


            if (!ProcessInfoCollect.GetInstance().SortDownProcess(Convert.ToInt32(id)))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "调准顺序失败！", "错误");
                return;
            }

            if (UpdateProcessInfo() < 0) return;
            Refresh();
        }

        private void OnUpProcess(object sender, RoutedEventArgs e)
        {
            Tool.XControl.XButton Btn = sender as Tool.XControl.XButton;
            string id = Btn.Tag.ToString();

            if (!ProcessInfoCollect.GetInstance().SortUPProcess(Convert.ToInt32(id)))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "调准顺序失败！", "错误");
                return;
            }

            if (UpdateProcessInfo() < 0) return;
            Refresh();
        }

        public int UpdateProcessInfo()
        {
            List<ProcessObjInfo> list = dcProcessInfo.Collection.ToList();

            Tool.Proto.ProtoProcessDetail proto = new Proto.ProtoProcessDetail();
            foreach (var obj in list)
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
            if (!proto.serialize(b_msg, ref offset)) return -1;
            int retval = -1;

            this.WaitEvent(()=> 
            {
                unsafe
                {
                    fixed (byte* l_msg = b_msg)
                    {
                        void* data = null;
                        int len = 0;
                        retval = NetInterface.NetPostSyncPackage(RobotId, (int)NetInterface.PKTTYPE.kAgvShellProto_UpdateProcessDetail, l_msg, proto.length(), &data, &len);
                       
                    }
                }

            },true,true);

            if (retval < 0)
            {
                TipsMessageEx.ShowTips(Tool.MainWindow.This, "更新软件配置失败", 18, 5, Colors.Green);
                return retval;
            }

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "更新软件配置成功", 18, 5, Colors.Green);
            return retval;
        }

        private void OnClickAddProcess(object sender, RoutedEventArgs e)
        {
            WinAddProcessService.Show(this);
        }

        private void OnClickApplyOther(object sender, RoutedEventArgs e)
        {
            List<ProcessObjInfo> list = dcProcessInfo.Collection.ToList();
            WindowSelectAGVService.Show(this, list);
        }
    }

    public static class WinProcessDetailService
    {
        public static void Show(this FrameworkElement Owner, Proto.ProtoProcessDetail list, string net_id)
        {
            WinProcessDetail.Show(Owner,list, net_id);
        }
    }
}
