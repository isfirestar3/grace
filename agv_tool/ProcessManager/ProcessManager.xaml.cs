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
using Tool.CommView;
using Tool.Net;
using Tool.Proto;
using Tool.RobotsHome;
using System.Threading;
using System.Windows.Threading;
using Tool.XControl;
using System.ComponentModel;

namespace Tool.ProcessManager
{
    public partial class ProcessManager : Page
    {
        static public ProcessManager This                = null;
        RobotInfoCollect             dcRobotInfo         = null;
        List<String>                 ProcessNameList     = new List<String>();
        DispatcherTimer              DispatcherTimer     = new DispatcherTimer();
        List<String>                 ProcessCheckObj     = new List<String>();
        List<String>                 RobotList           = new List<String>();
        XControl.XCheckBox           AllObj              = new XControl.XCheckBox();
        List<XCheckBox>              ProcessCheckObjList = new List<XCheckBox>();
        bool                         IsSelectAll         = false;
        ListSortDirection            DirectionSort = ListSortDirection.Ascending;
        public ProcessManager()
        {
            InitializeComponent();
            This = this;
            
            RefreshTable();
            Refresh();
            
            DispatcherTimer.Tick += new EventHandler(DispatcherTimerTick);
            DispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            DispatcherTimer.Start();
        }

        public void RefreshTable()
        {
            IsSelectAll = false;
            This.Dispatcher.Invoke(() => {
                This.WaitEvent(() =>
                {
                    NetInterface.NetQueryProcessesTableCallback();
                });
            });
        }
        public void Refresh()
        {
            dcRobotInfo = RobotInfoCollect.GetInstance();
            ProcessList.DataContext = dcRobotInfo;
            ProcessList.ItemsSource = dcRobotInfo.Collection.ToList().Where(p => p.NetState == true);
            SelectAllProcess.IsChecked = IsSelectAll;

            Sort(ProcessList, "Id", ListSortDirection.Ascending);
        }

        public void UnCheckObj()
        {
            ProcessCheckObj.Clear();
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = false);
        }

        private void DispatcherTimerTick(object sender, EventArgs e)
        {
            Refresh();
        }

        //根据回包动态添加列
        public void AddListColumn(string ep, List<ProcessTable> list)
        {
            this.Dispatcher.Invoke(() =>
            {
                ProcessNameList.Clear();
                ProcessView.Columns.Clear();
                ProcessCheckObj.Clear();
                ProcessTypeList.Children.Clear();
                //动态生成选择IP列
                Tool.XControl.GridViewColumnEx SeleIP = new GridViewColumnEx()
                {
                    Header = CreateCheckBox("",-1,0,0,0,0,true),
                    
                    Width = 40,
                    CellTemplate = (DataTemplate)Resources["SelectIP"],
                    
                };
                ProcessView.Columns.Add(SeleIP);

                //动态生成IP列
                Tool.XControl.GridViewColumnEx ID = new GridViewColumnEx()
                {
                    Header = "编号",
                    Width = 150,
                    CellTemplate = (DataTemplate)Resources["ID"]
                };
                ProcessView.Columns.Add(ID);

                //动态生成IP列
                Tool.XControl.GridViewColumnEx IP = new GridViewColumnEx()
                {
                    Header = "IP地址",
                    Width = 150,
                    CellTemplate = (DataTemplate)Resources["IP"]
                };
                ProcessView.Columns.Add(IP);

                //添加进程名称列
                int i = 1;
                foreach (var proc in list)
                {
                    if (i > 5) continue;

                    string index = "ProcessNo." + Convert.ToString(i);

                    if (ProcessNameList.Contains(proc.ProcessName.value_)) continue;
                   
                    ProcessNameList.Add(proc.ProcessName.value_);

                    Tool.XControl.GridViewColumnEx obj = new GridViewColumnEx()
                    {
                        Header = proc.ProcessName.value_,
                        Width = 150,
                        CellTemplate = (DataTemplate)Resources[index]
                    };
                    ProcessView.Columns.Add(obj);

                    XControl.XCheckBox obj_cb = CreateCheckBox(proc.ProcessName.value_, proc.ProcessId.value_,0,20,15,10);
                    obj_cb.Click += new RoutedEventHandler(OnSelectProcess);

                    ProcessTypeList.Children.Add((XControl.XCheckBox)obj_cb);
                    ProcessCheckObjList.Add(obj_cb);

                    i++;
                }
                RobotInfoCollect.GetInstance().UpdateProcessTable(ep, list);
            });
        }
        //创建CheckBox对象
        private XCheckBox CreateCheckBox(string name,int id,double left,double right,double top,double bottom,bool binding = false)
        {
            XControl.XCheckBox obj = new XControl.XCheckBox()
            {
               Content = name,
               Margin = CreateBorder(left, right,top, bottom),
               Tag = id,
               
            };
            if (binding)
            {
                Binding bind = new Binding("IsSelectedProcess");
                obj.SetBinding(XCheckBox.IsCheckedProperty, bind);
                obj.Click += new RoutedEventHandler(OnSelectAllIP);
                obj.Name = "AllCheckBox";
                AllObj = obj;
            }
            
            return obj;
        }
        //创建Border对象
        private Thickness CreateBorder(double left, double right, double top, double bottom)
        {
            Thickness obj = new Thickness()
            {
                Left = left,
                Top = top,
                Right= right,
                Bottom = bottom
            };
            return obj;
        }

        //更新进程状态
        public void UpdateProcessStatus(string ep, List<Tool.Proto.ProcessStatus> list)
        {
            RobotInfoCollect.GetInstance().UpdateProcessStatus(ep, list);
        }

        //重启  开启   停止 进程
        private void OnResetProcess(object sender, RoutedEventArgs e)
        {
            if(0==ProcessCheckObj.Count)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要操作的进程", "错误");
                return;
            }
            string id_list = "";
            foreach(var id in ProcessCheckObj)
            {
                id_list += id;
                id_list += "|";
            }

            string netid_list = "";
            foreach (var id in RobotList)
            {
                netid_list += id;
                netid_list += "|";
            }
            if (NetInterface.NetPostCodeAgvProcess(netid_list, id_list, Tool.RobotsHome.CmdAgvShell.cmd_restart_process)<0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "重启失败", "错误");
            }
            RobotList.Clear();
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "重启请求发送成功", 18, 3, Colors.Green);
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = false);
            AllObj.IsChecked = false;
        }

        private void OnStartProcess(object sender, RoutedEventArgs e)
        {
            if (0 == ProcessCheckObj.Count)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要操作的进程", "错误");
                return;
            }

            string id_list = "";
            foreach (var id in ProcessCheckObj)
            {
                id_list += id;
                id_list += "|";
            }
            string netid_list = "";
            foreach (var id in RobotList)
            {
                netid_list += id;
                netid_list += "|";
            }

            if (NetInterface.NetPostCodeAgvProcess(netid_list, id_list, Tool.RobotsHome.CmdAgvShell.cmd_start_process)<0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "启动失败", "错误");
            }
            RobotList.Clear();
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "启动请求发送成功", 18, 3, Colors.Green);
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = false);
            AllObj.IsChecked = false;
        }

        private void OnStopProcess(object sender, RoutedEventArgs e)
        {
            if (0 == ProcessCheckObj.Count)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请选择需要操作的进程", "错误");
                return;
            }

            string id_list = "";
            foreach (var id in ProcessCheckObj)
            {
                id_list += id;
                id_list += "|";
            }

            string netid_list = "";
            foreach (var id in RobotList)
            {
                netid_list += id;
                netid_list += "|";
            }

            if (NetInterface.NetPostCodeAgvProcess(netid_list, id_list, Tool.RobotsHome.CmdAgvShell.cmd_stop_process) < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "停止失败", "错误");
            }
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "停止请求发送成功", 18, 3, Colors.Green);
            RobotList.Clear();
            dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = false);
            AllObj.IsChecked = false;
        }


        private void OnSelectProcess(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
           
            string id = cb.Tag.ToString();
            if (true == cb.IsChecked)
            {
                ProcessCheckObj.Add(id);
            }
            else
            {
                ProcessCheckObj.Remove(id);
            }
        }

        private void OnSelectAllProcess(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            ProcessCheckObj.Clear();
            if (true == cb.IsChecked)
            {
                IsSelectAll = true;
                foreach (var obj in ProcessCheckObjList)
                {
                    obj.IsChecked = true;
                    ProcessCheckObj.Add(obj.Tag.ToString());
                }
            }
            else
            {
                IsSelectAll = false;
                foreach (var obj in ProcessCheckObjList)
                {
                    obj.IsChecked = false;
                }
            }
        }

        private void OnSelectAllIP(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                List<RobotInfo> list = dcRobotInfo.Collection.ToList();
                RobotList = list.Select(l => l.NetId.ToString()).ToList();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = true);
            }
            else
            {
                RobotList.Clear();
                dcRobotInfo.Collection.ToList().ForEach(p => p.IsSelectedProcess = false);
            }
        }
        private void OnSelectIP(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;

            string path = cb.Tag.ToString();
            if (true == cb.IsChecked)
            {
                RobotList.Add(path);
            }
            else
            {
                RobotList.Remove(path);
            }
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
            else if ("IP地址" == gch.Content.ToString())
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

            Sort(ProcessList, item, DirectionSort);
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
