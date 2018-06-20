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
using System.IO;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Tool.Net;
using Tool.CommView;
using System.Collections;
using System.Windows.Controls.Primitives;
using Tool.P2PConfig;
using System.Diagnostics;
using Tool.SelectAGV;
using Tool.FileHandle;
using Tool.RobotsHome;


namespace Tool.P2PConfig
{
    
    public partial class P2PConfig : Page
    {
        public static P2PConfig  This = null;
        List2List                listPanel = null;
        object                   PointItem = null;
        string                   ep;
        string                   ip;

        public P2PConfig(string ep, Proto.ProtoFolderList list, int SelectIndex)
        {
            This = this;
            InitializeComponent();
            InitEndpoint(ep);
            InitFileTree(list);
            InitOnlineList();
            RobotInfoCollect.GetInstance().AppendInitFileList(ep);
            listBoxIP.Focus();
            listBoxIP.SelectedIndex = SelectIndex;
        }
        private void InitEndpoint(string endpoint)
        {
            Char[] separartor = { ':' };
            string[] filearray = new string[2];
            filearray = endpoint.Split(separartor);
            ep = endpoint;
            ip = filearray[0];
            SetRemoteIPTitle(ip);
        }
        private void P2PConfigLoaded(object sender, RoutedEventArgs e)
        {
            listPanel = new List2List(ip, ep);
            FileTransPanel.FrontWarpper.Child = listPanel;
        }

        
        //初始化在线列表
        private void InitOnlineList()
         {
            List<RobotOnlineInfo> info = new List<RobotOnlineInfo>();
            RobotInfoCollect.GetInstance().FliterRobotOnline(ref info);

            listBoxIP.ItemsSource = info;
            listBoxHost.ItemsSource = info;
        }
        //初始化文件树
        public  void   InitFileTree(Proto.ProtoFolderList list)
        {
            String dirLocal = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref dirLocal);
            if (dirLocal == null) return;
            string pathLocal = dirLocal;

            String dirFolderSet;
            int posStart = pathLocal.IndexOf("standard");
            dirFolderSet = pathLocal.Substring(0, posStart);

            string pathSet = dirFolderSet + "FolderSet";

            if (Directory.Exists(pathSet))
            {
                FileOperator.DelectDir(pathSet);
            }

            FileOperator.CopyDir(pathLocal, pathSet);
            
            for(int i= 0; i<list.FolderList.Count;i++)
            {
                string path = pathSet + list.FolderList[i].FolderName.value_;

                if (!Directory.Exists(path))
                {
                    Directory.CreateDirectory(path);
                }
            }
            

            FolderTree.Items.Clear();

            TreeViewItem Nodes = new TreeViewItem()
            {
                Header = CreateTreeViewItemFolder(ip, "")
            };

            Nodes.ExpandSubtree();
            GetDirectoriesList(pathSet, Nodes);
            FolderTree.Items.Add(Nodes);
        }
        private object CreateTreeViewItemFolder(string AValue, string TagDir)
        {
            StackPanel stkPanl = new StackPanel()
            {
                Height = 25,
                Orientation = Orientation.Horizontal,
                VerticalAlignment = VerticalAlignment.Center,
                Tag = TagDir
            };
            Image img = new Image()
            {
                Width = 18,
                Height = 20,
                Source = new BitmapImage(new Uri("../Assets/文件夹关闭.png", UriKind.Relative))
            };

            stkPanl.Children.Add(img);
            Label lblHeader = new Label()
            {
                
                Content = AValue,
                Height = 28,
                VerticalAlignment = VerticalAlignment.Center,
                VerticalContentAlignment = VerticalAlignment.Center
            };
            stkPanl.Children.Add(lblHeader);
            
            return stkPanl;
        }
        private void   GetDirectoriesList(string fPath, TreeViewItem aNode)
        {
            //读取选择的路径下的文件和目录
            string[] directories = Directory.GetDirectories(fPath);//文件夹集合

            foreach (string pathString in directories)
            {
                DirectoryInfo info = new DirectoryInfo(pathString);
                string splitstr = "FolderSet";
                int pos = pathString.IndexOf(splitstr);
                string strRelativeDir = pathString.Substring(pos + splitstr.Length, pathString.Length - pos - splitstr.Length);
                TreeViewItem Node = new TreeViewItem()
                {
                    Header = CreateTreeViewItemFolder(info.Name, "."+strRelativeDir),
                };

                aNode.Items.Add(Node);
                aNode.Expanded += NodeExpanded;
                aNode.Collapsed += NodeCollapsed;

                if (Directory.GetDirectories(pathString) != null)
                {
                    
                    GetDirectoriesList(pathString, Node);
                }
            }

            //叶子节点同样需要添加响应
            if (0 == directories.Length)
            {
                aNode.Expanded += NodeExpanded;
                aNode.Collapsed += NodeCollapsed;
            }
        }


        //文件树节点关闭
        void NodeCollapsed(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)sender;

            var stackPanel = item.Header as StackPanel;
            var image = stackPanel.Children[0] as Image;
            image.Source = new BitmapImage(new Uri("../Assets/文件夹关闭.png", UriKind.Relative));

            e.Handled = true;

            if (stackPanel.Tag == null) return;

            string strRelativeDir = stackPanel.Tag.ToString();
            if(""== strRelativeDir)
            {
                listPanel.ClearUI();
                return;
            }
            var txt = stackPanel.Children[1] as Label;
            int pos = strRelativeDir.IndexOf(txt.Content.ToString());
            string pDir = strRelativeDir.Substring(0, pos);

            listPanel.NodeOperationInitList(pDir);
        }

        //文件树节点打开
        void NodeExpanded(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = (TreeViewItem)sender;
            var stackPanel = item.Header as StackPanel;
            var image = stackPanel.Children[0] as Image;
            image.Source = new BitmapImage(new Uri("../Assets/文件夹打开.png", UriKind.Relative));
            e.Handled = true;

            if (stackPanel.Tag == null) return;

            string strRelativeDir = stackPanel.Tag.ToString();
            if ("" == strRelativeDir)
            {
                listPanel.NodeOperationInitList("");
                return;
            }
            listPanel.NodeOperationInitList(strRelativeDir);
        }

       
        //刷新列表
        public void RefreshList()
        {
            if (null == listPanel) return;

            This.Dispatcher.Invoke(() =>
            {
                listPanel.RefreshList();
            });
        }
        private void OnRefreshList(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                This.RefreshList();
            });
        }


        //设置列表头
        public void SetRemoteIPTitle(string IP)
        {
            string str = string.Format("    IP:{0}配置",IP);
            TBRemoteIP.Text = str;
        }
        //设置路径显示
        public void SetFileDir(string IP,string path)
        {
            var dir_array= path.Split('\\');
            int size = dir_array.Count();

            string strPath = "";
            for(int i=0;i<size;i++)
            {
                if ("" == dir_array[i]) continue;
                strPath += " > ";
                strPath += dir_array[i];
            }


            string str = string.Format("    配置管理 > {0}配置文件表  {1}", IP, strPath);
            TBFileDir.Text = str;
        }


        //应用到多车
        private void OnAppToHost(object sender, RoutedEventArgs e)
        {
            List<RobotOnlineInfo> list = new List<RobotOnlineInfo>();
            RobotInfoCollect.GetInstance().FliterRobotOnline(ref list);
            This.AppToHost(list);
        }
        public void AppToHost(List<RobotOnlineInfo> Robotlist)
        {
            Tool.P2PConfig.List2List.This.AppToHost(Robotlist);
        }


        //点击IP或者主机名列表，刷新文件列表
        private void OnClickListIP(object sender, MouseButtonEventArgs e)
        {
            if (null == PointItem) return;

            ListBox obj = sender as ListBox;

            string ftsEndPoint = ((RobotOnlineInfo)PointItem).Fts;

            ChangeCarList(ftsEndPoint, obj.SelectedIndex);
        }
        private void OnClickListHost(object sender, MouseButtonEventArgs e)
        {
            if (null == PointItem) return;
            ListBox obj = sender as ListBox;

            string ftsEndPoint = ((RobotOnlineInfo)PointItem).Fts;

            ChangeCarList(ftsEndPoint, obj.SelectedIndex);

        }
        private void ChangeCarList(string ftsEndPoint, int SelectIndex)
        {
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
            Tool.MainWindow.This.NaviToPageConfig(ftsEndPoint, folderList, SelectIndex);
        }

        //当前目录后退
        private void OnStepBackFile(object sender, RoutedEventArgs e)
        {
            listPanel.NodeOperationStepBackFile();
        }


        //获取IP或者主机名列表焦点
        private void OnPreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            PointItem = GetElementFromPoint((ItemsControl)sender, e.GetPosition((ItemsControl)sender));
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

    }
}

