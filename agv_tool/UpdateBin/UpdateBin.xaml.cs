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
using Tool.RobotsHome;
using System.Xml.Linq;
using Tool.Net;
using Tool.CommView;
using Tool.XControl;
using System.Windows.Threading;
using System.Text.RegularExpressions;
using System.IO;
using System.ComponentModel;

namespace Tool.UpdateBin
{
    public enum VCU_MESSAGE
    {
        VCU_Type = 0,
        VCU_Version,
        VCU_CPU
    };

    public enum FRIMWARE_STATUS
    {
        kNormal = 0,//正常
        kBusy,     //操作中
        kRestarting, //正在重启

        kFailReadVCUInfo = -100, //读取VCU信息失败
        kFailRestart,           //重启失败
        kFailUpdate,            //升级失败
        kFailDownload,          //下载文件失败
        kFailCompare,           //文件比对失败
        kRequestTimeout         //请求超时
    };

    public partial class UpdateBin : Page
    {
        static public UpdateBin               This              = null;
        RobotInfoCollect                      dcRobotInfo       = null;
        Int32                                 FirmwareType      = -1;
        Int32                                 FirmwareLen       = 0;
        string                                FirmPath          = "";
        string                                FirmEndPoint      = "";
        List<string>                          SelectNetIdList   = new List<string>();
        List<string>                          SelectFTSList     = new List<string>();
        DispatcherTimer                       DispatcherTimer   = new DispatcherTimer();
        string                                DriverMode        = "0";
        ListSortDirection                     DirectionSort = ListSortDirection.Ascending;
        string                                SortFlag = "Id";
        public UpdateBin()
        {
            InitializeComponent();
            This = this;
            refresh();

            DispatcherTimer.Tick += new EventHandler(DispatcherTimerTick);
            DispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            DispatcherTimer.Start();

            NetInterface.NetRegistCallbackUpdateBin(CallBack);
            NetInterface.NetRegistCallbackTransStep(CallBackTransStep);
        }

        //定时刷新
        private void DispatcherTimerTick(object sender, EventArgs e)
        {
            refresh();
        }
        public void refresh()
        {
            dcRobotInfo = RobotInfoCollect.GetInstance();
            RobotUpdateList.DataContext = dcRobotInfo;
            RobotUpdateList.ItemsSource = dcRobotInfo.Collection.ToList().Where(p => p.NetState == true || p.BinUpdate == true);

           
            Sort(RobotUpdateList, SortFlag, ListSortDirection.Ascending);
        }




        /////////////////////重新获取软件版本信息//////////////////////////
        private int RefreshInfomtion(string net_id)
        {
            if ("" == FirmPath || "" == FirmEndPoint)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请先选择目标文件", "错误");
                return -1;
            }

            string id = TB_NodeId.Text.ToString();
            string can_bus = ComboxNodeMode.Text;
            int node_id;
            int can_bus_type;
            if (id == string.Empty)
            {
                node_id = 0;
            }
            else
            {
                node_id = Convert.ToInt32(id);
            }
            if (can_bus == string.Empty)
            {
                can_bus_type = 0;
            }
            else
            {
                can_bus_type = Convert.ToInt32(can_bus);
            }

            return NetInterface.NetGetBinDetailInfo(net_id, FirmwareType, FirmEndPoint, node_id, can_bus_type);
        }

        private void OnRefreshInfomtion(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();
            if (null == net_id) return;

            if (RefreshInfomtion(net_id) < 0)
            {
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "正在获取固件信息。。。", 18, 5, Colors.Green);
        }

        private void OnGetInfomtionBatch(object sender, RoutedEventArgs e)
        {
            foreach (string net_id in SelectNetIdList)
            {
                RefreshInfomtion(net_id);
            }
        }

        /////////////////////下载M核相机程序//////////////////////////
        private int DownLoadCore(string net_id,string path)
        {
            if ("" == FirmPath || "" == FirmEndPoint)
            {
                return -1;
            }

            return NetInterface.NetDownloadBinFiles(net_id, FirmwareType, FirmwareLen, path,FirmEndPoint);
        }

        private void OnDownLoad(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();
            if (null == net_id) return;

            System.Windows.Forms.FolderBrowserDialog folder = new System.Windows.Forms.FolderBrowserDialog();

            folder.ShowDialog();

            if ("" == folder.SelectedPath) return;

            if (DownLoadCore(net_id, folder.SelectedPath) < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请先选择目标文件", "错误");
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "正在下载固件。。。", 18, 5, Colors.Green);
        }

        private void OnDownloadBatch(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog folder = new System.Windows.Forms.FolderBrowserDialog();

            folder.ShowDialog();

            if ("" == folder.SelectedPath) return;

            foreach (string fts in SelectNetIdList)
            {
                DownLoadCore(fts, folder.SelectedPath);
            }
        }




        /////////////////////更新M核相机程序//////////////////////////
        private int UpdateCore(string fts_ep, bool isBatch = false)
        {
            if(!isBatch)
            {
                if (!File.Exists(FirmPath))
                {
                    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件不存在！", "错误");
                    return -1;
                }
            }
            

            if ("" == FirmPath || "" == FirmEndPoint)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请先选择目标文件", "错误");
                return -1;
            }


            return NetInterface.NetUpdateBinFiles(fts_ep, FirmwareType, FirmPath, FirmEndPoint);
        }

        private void OnUpdate(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string fts_ep = Btn.Tag.ToString();
            if (null == fts_ep) return;

            var fw = sender as FrameworkElement;
            if (null == fw) return;
            var info = fw.DataContext as Tool.RobotsHome.RobotInfo;
            //固件升级中状态进行中或者重启中，直接返回。
            if (1 == info.BinStatus || 2 == info.BinStatus)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "正在操作中，请稍后再试", "错误");
                return;
            }
             
            
            if (UpdateCore(fts_ep,false) < 0)
            {
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "正在更新固件。。。", 18, 5, Colors.Green);
        }

        private void OnUpdateBatch(object sender, RoutedEventArgs e)
        {
            if (!File.Exists(FirmPath))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件不存在！", "错误");
                return;
            }
            foreach (string fts_ep in SelectFTSList)
            {
                int retval = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindBinStatusByFTS(fts_ep);
                if (1 == retval || 2 == retval) continue;

                UpdateCore(fts_ep,true);
            }
        }


        /////////////////////重启M核相机程序//////////////////////////
        private int ResetProcessCore(string net_id)
        {
            return NetInterface.NetResetCore(net_id, FirmwareType, FirmEndPoint);
        }

        private void OnResetProcess(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();
            if (null == net_id) return;

            if (ResetProcessCore(net_id) < 0)
            {
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "正在重启。。。", 18, 5, Colors.Green);
        }

        private void OnResetProcessBatch(object sender, RoutedEventArgs e)
        {
            foreach (string net_id in SelectNetIdList)
            {
                ResetProcessCore(net_id);
            }
        }

        //单选多选
        private void OnSelectAllRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<RobotInfo> list = dcRobotInfo.Collection.Where(p => p.NetState == true).ToList();
                SelectNetIdList = list.Select(l => l.NetId.ToString()).ToList();
                SelectFTSList= list.Select(l => l.Fts.ToString()).ToList();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedBin = true);
            }
            else
            {
                SelectNetIdList.Clear();
                SelectFTSList.Clear();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedBin = false);
            }
        }

        private void OnSelectRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;

            string netId = cb.Tag.ToString();
            string fts = cb.CommandParameter.ToString();

            if (true == cb.IsChecked)
            {
                SelectNetIdList.Add(netId);
                SelectFTSList.Add(fts);
            }
            else
            {
                SelectNetIdList.Remove(netId);
                SelectFTSList.Remove(fts);
            }
        }


        //选择目标文件
        private void OnSelectTarget(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog ofd = new Microsoft.Win32.OpenFileDialog();
            ofd.DefaultExt = ".xml";
            ofd.Filter = "xml file|*.xml";


            ofd.ShowDialog();
            
            if (ofd.FileName == string.Empty) return;
            ReadXML(ofd.FileName);
            
            

            Tool.RobotsHome.RobotInfoCollect.GetInstance().ClearBinInfo();

            List<RobotInfo> list = new List<RobotInfo>();
            list = dcRobotInfo.Collection.ToList().Where(p => p.NetState == true || p.BinUpdate == true).ToList();

            string netidList = null;
            foreach (RobotInfo info in list)
            {
                if (info == null) continue;
                netidList += info.NetId + "|";
            }

            if (null == netidList) return;

            string id = TB_NodeId.Text.ToString();
            string can_bus = ComboxNodeMode.Text;
            int node_id;
            int can_bus_type;
            if(id == string.Empty)
            {
                node_id = 0;
            }
            else
            {
                node_id = Convert.ToInt32(id);
            }
            if(can_bus == string.Empty)
            {
                can_bus_type = 0;
            }
            else
            {
                can_bus_type = Convert.ToInt32(can_bus);
            }
        }
        public void ReadXML(string dir)
        {
            try
            {
                XDocument XDoc = XDocument.Load(dir);
                XElement rootNode = XDoc.Element("firmware");

                XElement node = rootNode.Element("modules_type");
                if(node!=null) TB_Model.Text = node.Value;

                node = rootNode.Element("firmware_path");
                if (node != null) {
                    TB_Bin.Text = System.IO.Path.GetFileName(node.Value);
                    FirmPath = node.Value;
                } 

                node = rootNode.Element("firmware_version");
                if (node != null) TB_Version.Text = node.Value;

                node = rootNode.Element("firmware_cpu_type");
                if (node != null) TB_CPU.Text = node.Value;

                node = rootNode.Element("firmware_type");
                if (node != null)  FirmwareType = Convert.ToInt32(node.Value);

                node = rootNode.Element("frimware_endpoint");
                if (node != null) FirmEndPoint = node.Value;

                node = rootNode.Element("firmware_length");
                if (node != null) FirmwareLen = Convert.ToInt32(node.Value);

            }
            catch (Exception)
            {

            }
        }



        //版本信息回调
        unsafe public static NetInterface.MsgCallbackUpdateBin CallBack = new NetInterface.MsgCallbackUpdateBin(
            (string ep, int type, string msg) =>
            {
                UpdateBin.This.Dispatcher.Invoke(() =>
                {
                    if (null == This) return;

                    Dictionary<string, Tool.RobotsHome.RobotInfo> robotinfo = new Dictionary<string, Tool.RobotsHome.RobotInfo>();
                    RobotInfoCollect.GetInstance().GetRobotinfoShell(ref robotinfo);

                    if (!robotinfo.ContainsKey(ep)) return;


                    switch ((VCU_MESSAGE)type)
                    {
                        case VCU_MESSAGE.VCU_CPU:
                            robotinfo[ep].ModelCPU = msg;
                            break;
                        case VCU_MESSAGE.VCU_Type:
                            robotinfo[ep].ModelName= msg;
                            break;
                        case VCU_MESSAGE.VCU_Version:
                            robotinfo[ep].ModelVersion = msg;
                            break;
                        default:
                            break;
                    }
                });
            }
        );
        //进度回调
        unsafe public static NetInterface.MsgCallbackTransStep CallBackTransStep = new NetInterface.MsgCallbackTransStep(
            (string ep, int type, int step) =>
            {
                UpdateBin.This.Dispatcher.Invoke(() =>
                {
                    if (null == This) return;

                    Dictionary<string, Tool.RobotsHome.RobotInfo> robotinfo = new Dictionary<string, Tool.RobotsHome.RobotInfo>();
                    RobotInfoCollect.GetInstance().GetRobotinfoShell(ref robotinfo);

                    if (!robotinfo.ContainsKey(ep)) return;
                    robotinfo[ep].BinStatus = type;
                    robotinfo[ep].ProgressRate = step;
                });
            }
         );

        private void OnChangeNodeMode(object sender, SelectionChangedEventArgs e)
        {
            string str = "";
            if (ComboxNodeMode.Text != "")
            {
                str = ((ComboBoxItem)(ComboxNodeMode.SelectedItem)).Content.ToString();
            }

            if ("" == str) return;
            DriverMode = str;
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


        /////////////////////更新自主驱动//////////////////////////
        private int UpdateDriver(string fts_ep,string driver_id,bool isBatch = false)
        {
            if (!isBatch)
            {
                if (!File.Exists(FirmPath))
                {
                    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件不存在！", "错误");
                    return -1;
                }
            }
            if(""==FirmPath || ""== FirmEndPoint)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请先选择目标文件", "错误");
                return -1;
            }
            return NetInterface.NetPushCanBinFile(fts_ep, Convert.ToInt32(driver_id), Convert.ToInt32(DriverMode),FirmPath, FirmEndPoint);
        }

        private void OnUpdateDriverBatch(object sender, RoutedEventArgs e)
        {
            if (!File.Exists(FirmPath))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件不存在！", "错误");
                return;
            }

            string id = TB_NodeId.Text.ToString();
            if (id == string.Empty)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请输入节点ID", "错误");
                return;
            }
            foreach (string fts_ep in SelectFTSList)
            {
                UpdateDriver(fts_ep, id,true);
            }
        }

        private void OnUpdateDriver(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string fts_ep = Btn.Tag.ToString();
            if (null == fts_ep) return;

            string id = TB_NodeId.Text.ToString();
            if (id == string.Empty)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请输入节点ID", "错误");
                return;
            }
            if (UpdateDriver(fts_ep,id,false) < 0)
            {
                return;
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "正在更新自主驱动。。。", 18, 5, Colors.Green);
        }

        private void GridViewColumnHeaderClickedHandler(object sender, RoutedEventArgs e)
        {
            GridViewColumnHeader gch = e.OriginalSource as GridViewColumnHeader;
            if (null == gch) return;
            string item = "";

            if ("编号" == gch.Content.ToString())
            {
                item = "Id";
                SortFlag = item;
            }
            else if ("IP地址" == gch.Content.ToString())
            {
                item = "EndPoint";
                SortFlag = item;
            }
            else if ("型号" == gch.Content.ToString())
            {
                item = "ModelName";
                SortFlag = item;
            }
            else if ("当前版本" == gch.Content.ToString())
            {
                item = "ModelVersion";
                SortFlag = item;
            }
            else if ("CPU型号" == gch.Content.ToString())
            {
                item = "ModelCPU";
                SortFlag = item;
            }

            if (string.Empty == item) return;


            if (ListSortDirection.Ascending == DirectionSort)
            {
                DirectionSort = ListSortDirection.Descending;
            }
            else
            {
                DirectionSort = ListSortDirection.Ascending;
            }

            Sort(RobotUpdateList, item, DirectionSort);
        }

        private void Sort(ListView lv, string sortBy, ListSortDirection direction)
        {

            ICollectionView dataView = System.Windows.Data.CollectionViewSource.GetDefaultView(lv.ItemsSource);
            dataView.SortDescriptions.Clear();
            SortDescription sd = new SortDescription(sortBy, direction);
            dataView.SortDescriptions.Add(sd);
            dataView.Refresh();
        }
    }
}
