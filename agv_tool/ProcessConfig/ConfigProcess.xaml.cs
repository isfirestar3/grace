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
using Tool.CommView;
using Tool.Net;
using System.Windows.Threading;
using System.IO;
using System.ComponentModel;

namespace Tool.ProcessConfig
{
    public partial class ProcessConfig : Page
    {
        static public ProcessConfig This = null;
        RobotInfoCollect dcRobotInfo = null;
        DispatcherTimer DispatcherTimer = new DispatcherTimer();
        ListSortDirection DirectionSort = ListSortDirection.Ascending;
        public ProcessConfig()
        {
            InitializeComponent();
            This = this;
            Refresh();

            DispatcherTimer.Tick += new EventHandler(DispatcherTimerTick);
            DispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            DispatcherTimer.Start();
        }

        public void Refresh()
        {
            dcRobotInfo = RobotInfoCollect.GetInstance();
            RobotList.DataContext = dcRobotInfo;
            RobotList.ItemsSource = dcRobotInfo.Collection.ToList().Where(p => p.NetState == true);

            Sort(RobotList, "Id", ListSortDirection.Ascending);
        }

        private void DispatcherTimerTick(object sender, EventArgs e)
        {
            Refresh();
        }

        private void OnClickedCheck(object sender, RoutedEventArgs e)
        {
            Tool.XControl.XButton Btn = sender as Tool.XControl.XButton;
            string net_id = Btn.Tag.ToString();

            Int32 retval = -1;
            Proto.ProtoProcessDetail list = new Proto.ProtoProcessDetail();

            this.WaitEvent(() =>
            {
                unsafe
                {
                    void* r_data = null;
                    int r_len = 0;
                    retval = NetInterface.NetPostSyncPackage(Convert.ToInt32(net_id), (int)NetInterface.PKTTYPE.kAgvShellProto_ProcessDetail,null,0, &r_data, &r_len);
                    if (retval < 0 || null==r_data) return;

                    byte[] ack_msg = null;

                    ack_msg = new byte[r_len];
                    using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)r_data, r_len))
                    {
                        ms.Read(ack_msg, 0, ack_msg.Length);
                    }

                    Tool.Proto.Unpackage<Proto.ProtoProcessDetail> info = new Proto.Unpackage<Proto.ProtoProcessDetail>(ack_msg);
                    if (info.build())
                    {
                        list = info.ob;
                    }
                    NetInterface.FreeDetail(r_data);
                }
            }, true, true);

            WinProcessDetail.Show(this, list,net_id);
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
            else if ("Mac" == gch.Content.ToString())
            {
                item = "MacAddr";
            }
            else if ("IP" == gch.Content.ToString())
            {
                item = "EndPoint";
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
