using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Tool.Net;
using Tool.CommView;
using System.Threading;
using System.IO;
using Tool.Other;
using System.Text.RegularExpressions;
using Tool.XControl;
using System.Windows.Media;
using System.ComponentModel;

namespace Tool.RobotsHome
{
    public enum CmdAgvShell
    {
        cmd_start_process = 0,
        cmd_stop_process,
        cmd_restart_process,
        cmd_restart_os
    };

    public enum ProcessStatus
    {
        kStop = 0,
        kRunning = 1
    };

    public partial class RobotsHome : Page
    {
        public static NetInterface.fn_link_closed_t fn_link_closed_t_ = new NetInterface.fn_link_closed_t(
            (Int32 netid) =>
            {
                RobotInfoCollect.GetInstance().UpdateNetStatus(netid, false);
                RobotInfoCollect.GetInstance().Reset(netid);
            }
        );

        public static NetInterface.fn_link_connected_t fn_link_connected_t_ = new NetInterface.fn_link_connected_t(
           (Int32 netid,Int32 status,string mac) =>
           {
               RobotInfoCollect.GetInstance().UpdateNetStatus(netid,true);
               RobotInfoCollect.GetInstance().UpdateMac(netid, mac);
           }
       );

        public static RobotsHome    This = null;
        List<string>                SelectRobotInfoList  =  new List<string>();
        ListViewPage<RobotInfo>     PageRobotInfo;
        Int32                       PageRobotCount    = 10;
        Int32                       RobotCount        = 0;
        RobotInfoCollect            dcRobotInfo       = null;
        ListSortDirection           DirectionSort = ListSortDirection.Ascending;
        public RobotsHome()
        {
            InitializeComponent();
            This = this;
            Refresh();
            NetInterface.NetRegsiterMsgCallback(NetInterface.NetWorkCallback);
        }
        //绑定
        public void Refresh()
        {
            dcRobotInfo = RobotInfoCollect.GetInstance();
            RobotList.DataContext = dcRobotInfo;
            RobotCount = dcRobotInfo.Collection.ToList().Count;
            RefrshRobotPageInfo(dcRobotInfo.Collection.ToList(),PageRobotCount);
        }
        //分页
        public void RefrshRobotPageInfo(List<RobotInfo> list,Int32 EveryPageCount)
        {
            PageRobotInfo = new Tool.ListViewPage<RobotInfo>(list, EveryPageCount);
            PageRobotCount = EveryPageCount;
            this.RobotList.ItemsSource = PageRobotInfo.GetPageData(JumpOperation.GoHome);
            RobotList.Items.Refresh();

            Sort(RobotList, "Id", ListSortDirection.Ascending);
        }
        //改变单页个数
        private void OnChangePageNum(object sender, SelectionChangedEventArgs e)
        {
            ComboBox PageNum = sender as ComboBox;
            string str = PageNum.SelectedItem.ToString();
        }
        //下一页
        private void OnNextPage(object sender, RoutedEventArgs e)
        {
            this.RobotList.ItemsSource = PageRobotInfo.GetPageData(JumpOperation.GoNext);
        }
        //前一页
        private void OnPreviousPage(object sender, RoutedEventArgs e)
        {
            this.RobotList.ItemsSource = PageRobotInfo.GetPageData(JumpOperation.GoPrePrevious);
        }
        //单选
        private void OnSelectAllRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                SelectRobotInfoList = dcRobotInfo.Collection.ToList().Select(l => l.EndPoint).ToList();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = true);
            }
            else
            {
                SelectRobotInfoList.Clear();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = false);
            }
        }
        //全选
        private void OnSelectRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            string str = cb.Tag.ToString();
            if (true==cb.IsChecked)
            {
                SelectRobotInfoList.Add(str);
            }
            else
            {
                SelectRobotInfoList.Remove(str);
            }
        }
        //查看文件
        private void OnClickConfigPage(object sender, MouseButtonEventArgs e)
        {
            TextBlock text = sender as TextBlock;
            string ftsEndPoint = text.Tag.ToString();

            Int32 retval = -1;
            Proto.ProtoFolderList folderList = new Proto.ProtoFolderList();
            this.WaitEvent(() =>
            {
                unsafe
                {
                    void* r_data = null;
                    int r_len = 0;
                    retval = NetInterface.NetGetRomoteListDir(ftsEndPoint, "./", &r_data, &r_len);

                    byte[] ack_msg = null;

                    ack_msg = new byte[r_len];
                    using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)r_data, r_len))
                    {
                        ms.Read(ack_msg, 0, ack_msg.Length);
                    }

                    Tool.Proto.Unpackage<Proto.ProtoFolderList> list = new Proto.Unpackage<Proto.ProtoFolderList>(ack_msg);
                    if (list.build())
                    {
                        folderList = list.ob;
                    }
                    NetInterface.FreeDetail(r_data);
                }
            }, true, true);

            if (retval < 0)
            {
                string str = string.Format("获取文件信息失败,错误码:{0}", retval);
                this.ShowNotify(str, "错误");
                return;
            }
            Tool.MainWindow.This.NaviToPageConfig(ftsEndPoint,folderList,0);
        }


        ///////////////////////////连接操作//////////////////////////////////
        private void ConnectHost(string endpoint)
        {
            this.Dispatcher.Invoke(() =>
            {
                Int32 robotId = -1;
                robotId = NetInterface.init(fn_link_closed_t_, fn_link_connected_t_, endpoint);
                if (robotId < 0)
                {
                    this.ShowNotify("初始化网络失败", "错误");
                    return;
                }
                RobotInfoCollect.GetInstance().UpdateNetId(endpoint, robotId);


                if (NetInterface.Connect(robotId, endpoint) < 0)
                {
                    this.ShowNotify("网络连接失败", "错误");
                    return;
                }
            });
        }
        private void OnConnectHost(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string endpoint = Btn.Tag.ToString();
            if (null == endpoint) return;
            ConnectHost(endpoint);
        }
        private void OnConnnectBatch(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                this.WaitEvent(() =>
                {
                    int count = 0;
                    int total = SelectRobotInfoList.Count();
                    foreach (string ep in SelectRobotInfoList)
                    {
                        if (RobotInfoCollect.GetInstance().IsConnectedHost(ep)) continue;
                        ConnectHost(ep);
                        Thread.Sleep(10);
                        WaitWindow.ChangeCallback(0, (++count) * 100 / total);
                    }
                });

            });
        }


        ///////////////////////////断开操作//////////////////////////////////
        private void DisconectHost(Int32 RobotId)
        {
            if (NetInterface.Disconnect(RobotId) < 0)
            {
                return;
            }

            RobotInfoCollect.GetInstance().UpdateNetStatus(RobotId, false);
        }
        private void OnDisconnectHost(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            Int32 RobotId = Convert.ToInt32(Btn.Tag);

            DisconectHost(RobotId);
        }
        private void OnDisconnnectBatch(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                this.WaitEvent(() =>
                {
                    int count = 0;
                    int total = SelectRobotInfoList.Count();

                    foreach (string ep in SelectRobotInfoList)
                    {
                        Int32 netid = RobotInfoCollect.GetInstance().FindNetId(ep);
                        DisconectHost(netid);
                        Thread.Sleep(100);
                        WaitWindow.ChangeCallback(0, (++count) * 100 / total);
                    }
                });

            });
        }




        ////////////////////////////////////agvshell自更新//////////////////////////////
        private void OnUpdateShell(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            
            string net_id = Btn.Tag.ToString();

            Microsoft.Win32.OpenFileDialog ofd = new Microsoft.Win32.OpenFileDialog();
            ofd.DefaultExt = ".tar";
            ofd.Filter = "tar file|*.tar";
            ofd.ShowDialog();

            if (ofd.FileName == string.Empty) return;

            NetInterface.NetUpdateAgvShell(net_id, ofd.FileName);
        }
        
        private void OnUpdateShellBatch(object sender, RoutedEventArgs e)
        {

            string list = null;
            foreach (string ep in SelectRobotInfoList)
            {
                if (!Tool.RobotsHome.RobotInfoCollect.GetInstance().IsConnectedHost(ep)) continue;
                list += Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetId(ep) + "|";
            }

            if (null == list)
            {
                this.ShowNotify("请选择需要一键升级的车辆", "提示");
                return;
            }

            Microsoft.Win32.OpenFileDialog ofd = new Microsoft.Win32.OpenFileDialog();
            ofd.DefaultExt = ".tar";
            ofd.Filter = "tar file|*.tar";
            ofd.ShowDialog();

            if (ofd.FileName == string.Empty) return;

            NetInterface.NetUpdateAgvShell(list, ofd.FileName);
            WinShellProcessPercService.Show(this);
        }


        ////////////////////////////////////NTP服务/////////////////////////////////////
        private void OnNTP(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();

            WinNTPService.Show(This, net_id);
        }
        private void OnNTPBatch(object sender, RoutedEventArgs e)
        {
            string list = null;
            foreach (string ep in SelectRobotInfoList)
            {
                list += Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetId(ep) + "|";
            }
            if (null == list)
            {
                this.ShowNotify("请选择需要执行NTP同步的车辆", "提示");
                return;
            }
            WinNTPService.Show(This, list);

        }


        ////////////////////////////////////VCU自动重启/////////////////////////////////
        private void OnClickConfigVCUEnable(object sender, MouseButtonEventArgs e)
        {
            TextBlock Btn = sender as TextBlock;
            string net_id = Btn.Tag.ToString();

            RobotInfo obj = (RobotInfo)Btn.DataContext;

            WinVCURebootService.Show(This, Convert.ToInt32(net_id), obj.VCUStatus);
        }




        ////////////////////////////////加锁解锁文件操作////////////////////////////////
        private void LockFile(string net_id, bool isBatch = false)
        {
            this.Dispatcher.Invoke(() =>
            {
                int retval = -1;
                this.WaitEvent(() =>
                {
                    unsafe
                    {
                        void* r_data = null;
                        int r_len = 0;
                        int arg = 1;// 加锁
                        retval = NetInterface.NetPostSyncPackage(Convert.ToInt32(net_id), (int)NetInterface.PKTTYPE.kAgvShellProto_ModifyMutex, &arg, 4, &r_data, &r_len);
                       
                        if (null == r_data) return;
                        NetInterface.FreeDetail(r_data);
                    }
                }, true, true);

               
            });
        } 
        private void OnLockFiles(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();

            LockFile(net_id);
        }
        private void UnLockFile(string net_id, bool isBatch = false)
        {
            this.Dispatcher.Invoke(() =>
            {
                int retval = -1;
                this.WaitEvent(() =>
                {
                    unsafe
                    {
                        void* r_data = null;
                        int r_len = 0;
                        int arg = 0;// 解锁
                        retval = NetInterface.NetPostSyncPackage(Convert.ToInt32(net_id), (int)NetInterface.PKTTYPE.kAgvShellProto_ModifyMutex, &arg, 4, &r_data, &r_len);

                        if (null == r_data) return;
                        NetInterface.FreeDetail(r_data);
                    }
                }, true, true);

                if (retval < 0)
                {
                    if (!isBatch) this.ShowNotify("解锁失败", "提示");
                    return;
                }

                if (!isBatch) NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "解锁完成", "提示");
            });
        }
        private void OnLockUnFiles(object sender, MouseButtonEventArgs e)
        {
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();
            
            UnLockFile(net_id);
        }
        private void OnLockBatch(object sender, RoutedEventArgs e)
        {
            if (0 == SelectRobotInfoList.Count)
            {
                this.ShowNotify("请选择需要加锁的车辆", "提示");
                return;
            }

            foreach (string ep in SelectRobotInfoList)
            {
                if (!RobotInfoCollect.GetInstance().IsConnectedHost(ep)) continue;
                int net_id = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetId(ep);
                LockFile(net_id.ToString(), true);
            }
        }
        private void OnUnLockBatch(object sender, RoutedEventArgs e)
        {
            if (0 == SelectRobotInfoList.Count)
            {
                this.ShowNotify("请选择需要解锁的车辆", "提示");
                return;
            }
            foreach (string ep in SelectRobotInfoList)
            {
                if (!RobotInfoCollect.GetInstance().IsConnectedHost(ep)) continue;
                int net_id = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetId(ep);
                UnLockFile(net_id.ToString(), true);
            }
        }


        ////////////////////////////////备份文件操作///////////////////////////////////
        private void BackUpFiles(List<string> list)
        {
            System.Windows.Forms.FolderBrowserDialog folder = new System.Windows.Forms.FolderBrowserDialog();

            folder.ShowDialog();

            if ("" == folder.SelectedPath) return;


            Tool.Proto.ProtoBackup Msg = new Proto.ProtoBackup();
            Msg.MSG.value_ = folder.SelectedPath;

            foreach (string ep in list)
            {
                if (ep == String.Empty) continue;
                if (!Tool.RobotsHome.RobotInfoCollect.GetInstance().IsConnectedHost(ep)) continue;
                int net_id = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetId(ep);
                int car_id = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindCarId(ep);
                Tool.Proto.ProtoObj value =  new Proto.ProtoObj();
                value.ID.value_ = net_id;
                value.CarID.value_ = car_id;
                Msg.ObjList.Add(value);
            }

            byte[] b_msg = new byte[Msg.length()];
            Int32 offset = 0;
            Msg.serialize(b_msg, ref offset);

            unsafe
            {
                fixed (byte* l_msg = b_msg)
                {
                    NetInterface.NetPostBackupFile(l_msg, Msg.length());
                }
            }
            
            WinShellProcessPercService.Show(this);
        }
        private void OnBackUpFiles(object sender, MouseButtonEventArgs e)
        {
            List<string> list = new List<string>();
            Tool.XControl.ImageButton Btn = sender as Tool.XControl.ImageButton;
            string net_id = Btn.Tag.ToString();

            string ep  = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetEP(Convert.ToInt32(net_id));
            list.Add(ep);
            BackUpFiles(list);
        }
        private void OnBackUpBatch(object sender, RoutedEventArgs e)
        {
            if (0 == SelectRobotInfoList.Count)
            {
                this.ShowNotify("请选择需要备份的车辆", "提示");
                return;
            }

            BackUpFiles(SelectRobotInfoList);
        }



        private void OnPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            if (e.Text.ToString()==string.Empty)
            {
                Refresh();
                return;
            }
            Regex re = new Regex("[^0-9.-]+");
            e.Handled = re.IsMatch(e.Text);
            if (e.Handled) return;
        }

        private void OnChangeTextInput(object sender, TextChangedEventArgs e)
        {
            if (SelectIP.Text.ToString() == string.Empty)
            {
                Refresh();
                return;
            }

            string str = SelectIP.Text.ToString();
            RefrshRobotPageInfo(dcRobotInfo.Collection.ToList().Where(p => p.EndPoint.Contains(SelectIP.Text.ToString()) == true).ToList(), PageRobotCount);
        }

        private void OnShowOnline(object sender, RoutedEventArgs e)
        {
            
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                RefrshRobotPageInfo(dcRobotInfo.Collection.ToList().Where(p =>p.NetState == true).ToList(), PageRobotCount);
            }
            else
            {
                Refresh();
            }
            
        }

        public void SetCheck()
        {
            AllCheckBox.IsChecked = false;
            SelectRobotInfoList.Clear();
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = false);
        }

        private void GridViewColumnHeaderClickedHandler(object sender, RoutedEventArgs e)
        {
            GridViewColumnHeader gch = e.OriginalSource as GridViewColumnHeader;
            if (null == gch) return;
            string item = "";

            if ("编号" == gch.Content.ToString())
            {
                item = "Id";
            }
            else if ("编号" == gch.Content.ToString())
            {
                item = "Id";
            }
            else if ("MAC" == gch.Content.ToString())
            {
                item = "MacAddr";
            }
            else if ("IP" == gch.Content.ToString())
            {
                item = "EndPoint";
            }
            else if ("连接状态" == gch.Content.ToString())
            {
                item = "NetState";
            }
            else if ("自动重启" == gch.Content.ToString())
            {
                item = "VCUStatus";
            }
            else if ("车载版本" == gch.Content.ToString())
            {
                item = "VersionInfo";
            }

            if (string.Empty == item) return;
           

            if(ListSortDirection.Ascending == DirectionSort)
            {
                DirectionSort = ListSortDirection.Descending;
            }
            else
            {
                DirectionSort = ListSortDirection.Ascending;
            }

            Sort(RobotList, item, DirectionSort);
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
