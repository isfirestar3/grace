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
using Tool.P2PConfig;
using System.IO;
using System.ComponentModel;

namespace Tool.CarHouse
{
   
    public partial class CartHousePage : Page
    {
        public static CartHousePage   This                =   null;
        public List<RobotInfo>        RobotInfoList       =   new List<RobotInfo>();
        RobotInfoCollect              dcRobotInfo         =   null;
        List<string>                  SelectEditRobotList =   new List<string>();
        object                        PointItem           =   null;
        ListSortDirection             DirectionSort       =   ListSortDirection.Ascending;
        public CartHousePage()
        {
            InitializeComponent();
            This = this;
            Refresh();
        }

        public void Refresh()
        {
            dcRobotInfo = RobotInfoCollect.GetInstance();
            RobotManageListView.DataContext = dcRobotInfo;
            RobotInfoList = dcRobotInfo.Collection.ToList();
            RobotManageListView.ItemsSource = RobotInfoList;

            Sort(RobotManageListView, "Id", ListSortDirection.Ascending);
        }

        private void OnSelectAllRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<RobotInfo> List = dcRobotInfo.Collection.ToList();
                SelectEditRobotList = List.Select(l => l.EndPoint).ToList();
                SelectEditRobotList = SelectEditRobotList.Where(s => s != null).ToList();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = true);
            }
            else
            {
                SelectEditRobotList.Clear();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = false);
            }
        }

        private void OnSelectRobots(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            string str = cb.Tag.ToString();
            if (true == cb.IsChecked)
            {
                SelectEditRobotList.Add(str);
            }
            else
            {
                SelectEditRobotList.Remove(str);
            }
        }

        private void OnAddCars(object sender, RoutedEventArgs e)
        {
            CarProduceWin.Show(Tool.MainWindow.This);
        }

        private void OnEditCars(object sender, RoutedEventArgs e)
        {
            if (!BtnAdd.IsEnabled) return;

            if (null==PointItem)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要编辑的车辆", "提示");
                return;
            }

            if (((Tool.RobotsHome.RobotInfo)(PointItem)).NetState)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "该车已经连接无法编辑", "提示");
                return;
            }

            CarEditWin.Show(Tool.MainWindow.This,((Tool.RobotsHome.RobotInfo)PointItem));
        }

        private void OnDelCars(object sender, RoutedEventArgs e)
        {
            DeleteFiles(SelectEditRobotList);
        }

        private void OnClickManageRobotList(object sender, MouseButtonEventArgs e)
        {
            if (!BtnAdd.IsEnabled) return;
            var SelectedItem = RobotManageListView.SelectedItem;
            if (null == SelectedItem) return;

            RobotInfo info =  SelectedItem as Tool.RobotsHome.RobotInfo;
          
            if (info.NetState)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "该车已经连接无法编辑", "提示");
                return;
            }
            CarEditWin.Show(Tool.MainWindow.This, info);
        }

        private object GetElementFromPoint(ItemsControl itemsControl, Point point)
        {
            UIElement element = itemsControl.InputHitTest(point) as UIElement;
            while (element != null)
            {
                if (element == itemsControl)
                    return null;
                object item = itemsControl.ItemContainerGenerator.ItemFromContainer(element);
                if (!item.Equals(DependencyProperty.UnsetValue))
                    return item;
                element = (UIElement)VisualTreeHelper.GetParent(element);
            }
            return null;
        }

        private void OnPreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            PointItem = GetElementFromPoint((ItemsControl)sender, e.GetPosition((ItemsControl)sender));
        }

        private void OnClickConfigPage(object sender, MouseButtonEventArgs e)
        {
            TextBlock text = sender as TextBlock;
            string ftsIP = text.Tag.ToString();

            Int32 retval = -1;
            Proto.ProtoFolderList folderList = new Proto.ProtoFolderList();
            this.WaitEvent(() =>
            {
                unsafe
                {
                    void* r_data = null;
                    int r_len = 0;
                    retval = NetInterface.NetGetRomoteListDir(ftsIP, "./", &r_data, &r_len);

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
            Tool.MainWindow.This.NaviToPageConfig(ftsIP, folderList, 0);
        }

        public void SetCheck()
        {
            AllCheckBox.IsChecked = false;
            SelectEditRobotList.Clear();
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = false);
        }

        private void OnMenuEditFile(object sender, RoutedEventArgs e)
        {
            if (!BtnAdd.IsEnabled) return;
            if (null == PointItem)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要编辑的车辆", "提示");
                return;
            }

            if (((Tool.RobotsHome.RobotInfo)(PointItem)).NetState)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "该车已经连接无法编辑", "提示");
                return;
            }

            CarEditWin.Show(Tool.MainWindow.This, ((Tool.RobotsHome.RobotInfo)PointItem));
        }

        private void DeleteFiles(List<string> list)
        {
            

            List<string> listFilter = new List<string>();
            foreach (string ep in list)
            {
                if (Tool.RobotsHome.RobotInfoCollect.GetInstance().IsConnectedHost(ep))
                {
                    continue;
                }
                listFilter.Add(ep);
            }

            if (0 == listFilter.Count)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "无可删除的车辆", "提示");
                return;
            }

            string str = string.Format("请确认是否删除？");
            MessageBoxResult isOverWrite = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, str, "提示", System.Windows.MessageBoxButton.YesNo);
            if (isOverWrite == MessageBoxResult.No)
            {
                return;
            }

            if (!Tool.RobotsHome.RobotInfoCollect.GetInstance().RemoveRobotInfo(listFilter))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "删除信息失败", "提示");
                return;
            }

            Tool.RobotsHome.RobotsHome.This.Refresh();
            Tool.CarHouse.CartHousePage.This.Refresh();

            SelectEditRobotList.Clear();
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelected = false);
            AllCheckBox.IsChecked = false;
        }

        private void OnMenuDelFile(object sender, RoutedEventArgs e)
        {
            if (!BtnAdd.IsEnabled) return;
            List<string> tmpList = new List<string>();

            if (null == PointItem)
            {
                tmpList = SelectEditRobotList;
            }

            else
            {
                if (((RobotInfo)PointItem).IsSelected)
                {
                    tmpList = SelectEditRobotList;
                }
                else
                {
                    tmpList.Add(((RobotInfo)PointItem).EndPoint);
                }
            }

            DeleteFiles(tmpList);
        }

        private void OnChangeMode(object sender, RoutedEventArgs e)
        {
            if (This == null) return;
            if (null == dcRobotInfo) dcRobotInfo = RobotInfoCollect.GetInstance();

            List<int> list = dcRobotInfo.Collection.ToList().Select(l => l.NetId).ToList();
            foreach (int netid in list)
            {
                if (netid < 0) continue;
                Tool.Net.NetInterface.Disconnect(netid);
            }

            if (true == RDYes.IsChecked)
            {
                Tool.Net.NetInterface.OpenDhcp(false);
                Tool.RobotsHome.RobotInfoCollect.GetInstance().CleanRobotObj();
                Tool.RobotsHome.RobotInfoCollect.GetInstance().ReadXML();
                Tool.RobotsHome.RobotsHome.This.Refresh();
                
                BtnAdd.IsEnabled = true;
                BtnDel.IsEnabled = true;
                MenuEdit.IsEnabled = true;
                MenuDel.IsEnabled = true;
                Refresh();
            }

            else if (true== RDNo.IsChecked)
            {
                int ret = Tool.Net.NetInterface.OpenDhcp(true);
                if (ret < 0)
                {
                    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "打开DHCP失败", "提示");
                    return;
                }
                Tool.RobotsHome.RobotInfoCollect.GetInstance().CleanRobotObj();
                Refresh();

                this.Dispatcher.Invoke(() => {
                    this.WaitEvent(() =>
                    {
                        Tool.RobotsHome.RobotInfoCollect.GetInstance().FromDHCP();
                    }, true, true);
                });
                


                Tool.RobotsHome.RobotsHome.This.Refresh();
                //int ret = Tool.Net.NetInterface.OpenDhcp(true);
                //if (ret < 0)
                //{
                //    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "打开DHCP失败", "提示");
                //    return;
                //}

                BtnAdd.IsEnabled = false;
                BtnDel.IsEnabled = false;
                MenuEdit.IsEnabled = false;
                MenuDel.IsEnabled = false;
                Refresh();
            }
        }

        public void UpdateRobotList()
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                this.WaitEvent(() =>
                {
                    Tool.RobotsHome.RobotInfoCollect.GetInstance().UpdateDHCP();
                }, true, true);
                Tool.RobotsHome.RobotsHome.This.Refresh();
                Refresh();
            }));

           
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

            if (string.Empty == item) return;


            if (ListSortDirection.Ascending == DirectionSort)
            {
                DirectionSort = ListSortDirection.Descending;
            }
            else
            {
                DirectionSort = ListSortDirection.Ascending;
            }

            Sort(RobotManageListView, item, DirectionSort);
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
