using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows;
using System.Windows.Automation.Peers;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using Tool.CommView;
using Tool.Net;
using Tool.SelectAGV;
using Tool.XControl;


namespace Tool.P2PConfig
{
    public enum FileTYPE
    {
        Local = 1,
        Remote = 2
    }

    public partial class List2List : UserControl
    {
        public static List2List     This                        = null;
        //list元素管理对象
        FileDetailInfoLocalCollect  dcLocal                     = null;   
        FileDetailInfoRemoteCollect dcRemote                    = null;
        //真实文件信息
        List<FileDetailInfo>        LocalList                   = new List<FileDetailInfo>(); 
        List<FileDetailInfo>        RemoteList                  = new List<FileDetailInfo>();
        //选择的文件列表
        List<TransFilesInfo>        SelectLocalList             = new List<TransFilesInfo>();       
        List<TransFilesInfo>        SelectRemoteList            = new List<TransFilesInfo>();
        //对比文件信息
        List<FileDetailInfo>        RemoteRelativeList          = new List<FileDetailInfo>(); 
        List<FileDetailInfo>        LocalRelativeList           = new List<FileDetailInfo>();

        //endpoint
        string EP;
        string IP;

        //选中的ListViewItem
        object PointItem   = null;

        //当前所在的相对路劲
        string DirRelative = ".\\";

        string BaseDir = System.AppDomain.CurrentDomain.BaseDirectory;
        public List2List(string ip, string ep)
        {
            InitializeComponent();
            This = this;
            NetInterface.NetRegistPullCallback(WaitWindow.ChangeCallback);
            EP = ep;
            IP = ip;
            RefreshList();
        }

        //遍历标准文件夹
        private void GetDirList(string fPath)
        {
            if (!Directory.Exists(fPath)) return;

            string[] directories = Directory.GetDirectories(fPath);
            foreach (string filePath in directories)
            {
                DirectoryInfo dirinfo = new DirectoryInfo(filePath);
                
                FileDetailInfo Info = new FileDetailInfo();
                Info.FileName = dirinfo.Name;

                Info.IsEnable = true;
                if (Directory.Exists(filePath)) Info.IsDir = true;
                string splitstr = "standard";
                int pos = filePath.IndexOf(splitstr);

                Info.LocalDir = filePath.Substring(pos + splitstr.Length, filePath.Length - pos - splitstr.Length);
                UInt32 CRC32 = 0;
                NetInterface.NetCRC32(filePath, ref CRC32);
                Info.LocalCRC = Convert.ToString(CRC32); ;
                Info.MotifyLocalTime = dirinfo.LastWriteTime.ToString();
                LocalList.Add(Info);

            }
            GetFileList(fPath);
        }
        private void GetFileList(string fPath)
        {
            string[] files = Directory.GetFiles(fPath, "*");

            foreach (string filePath in files)
            {
                FileDetailInfo Info = new FileDetailInfo();
                Info.FileName = System.IO.Path.GetFileName(filePath);
                FileInfo fi = new FileInfo(filePath);
                UInt32 CRC32 = 0;
                NetInterface.NetCRC32(filePath, ref CRC32);

                Info.IsDir = false;
                Info.IsEnable = true;
                string splitstr = "standard";
                int pos = filePath.IndexOf(splitstr);

                Info.LocalDir = filePath.Substring(pos + splitstr.Length, filePath.Length - pos - splitstr.Length);
                Info.LocalCRC = Convert.ToString(CRC32);
                Info.MotifyLocalTime = fi.LastWriteTime.ToString();
                LocalList.Add(Info);
            }
        }

        //初始化Local文件列表信息
        private void initlocalList()
        {
            String dir = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref dir);
            if (dir == null) return;

            string path = dir + DirRelative;
            GetDirList(path);
        }

        //初始化Remote文件列表信息
        private void InitRemoteList()
        {
            Int32 retval = -1;
            Proto.ProtoFileList fileList = new Proto.ProtoFileList();

            this.Dispatcher.Invoke(() => {
                unsafe
                {
                    void* r_data = null;
                    int r_len = 0;
                    retval = NetInterface.NetQueryCurrentCatalog(EP, DirRelative, &r_data, &r_len);
                    if (r_len==0)
                    {
                        return;
                    }

                    byte[] ack_msg = null;
                    ack_msg = new byte[r_len];
                    using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)r_data, r_len))
                    {
                        ms.Read(ack_msg, 0, ack_msg.Length);
                    }

                    Tool.Proto.Unpackage<Proto.ProtoFileList> list = new Proto.Unpackage<Proto.ProtoFileList>(ack_msg);
                    if (list.build())
                    {
                        fileList = list.ob;
                    }

                    NetInterface.FreeDetail(r_data);
                }
            });
           
            if (retval<-1)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "获取车载文件列表失败", "错误");
                return;
            }

            DateTime dtStart = TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1601, 1, 1));
            foreach ( var obj in fileList.FileListList)
            {
                FileDetailInfo Info = new FileDetailInfo();
                Info.FileName = obj.FileName.value_;
                Info.IsEnable = true;
                if (".\\" == DirRelative)
                {
                    Info.RemoteDir = DirRelative + Info.FileName;
                }
                else
                {
                    Info.RemoteDir = DirRelative + "\\" + Info.FileName;
                }
                
                if (0==obj.FileType.value_)
                {
                    Info.IsDir = true;
                }

                Info.RemoteCRC = obj.FileCRC32.value_.ToString();
                DateTime targetDt = dtStart.AddTicks((long)obj.FileModifyTime.value_);
                Info.MotifyRemoteTime = targetDt.ToString();
                Info.FileSize = obj.FileSize.value_;
                RemoteList.Add(Info);
            }
        }

        //计算文件比对信息
        private void InitRelativeList()
        {
            FileClassify.InitFileClassify(LocalList, RemoteList, ref LocalRelativeList, ref RemoteRelativeList);
        }
        
        //绑定界面元素
        private void Binging()
        {
            // 初始化本地文件信息，绑定list
            dcLocal = FileDetailInfoLocalCollect.GetInstance();
            ObservableCollection<FileDetailInfo> localcollection = new ObservableCollection<FileDetailInfo>(LocalRelativeList);
            dcLocal.Collection = localcollection;
            LocalListView.DataContext = dcLocal;
            this.LocalListView.ItemsSource = dcLocal.Collection.ToList().OrderByDescending(a => a.IsDir);

            //初始化远端文件信息，绑定list
            dcRemote = FileDetailInfoRemoteCollect.GetInstance();
            ObservableCollection<FileDetailInfo> remotecollection = new ObservableCollection<FileDetailInfo>(RemoteRelativeList);
            dcRemote.Collection = remotecollection;
            RemoteListView.DataContext = dcRemote;
            this.RemoteListView.ItemsSource = dcRemote.Collection.ToList().OrderByDescending(a => a.IsDir);
        }

        //清除界面元素
        public void ClearUI()
        {
            LocalList.Clear();
            RemoteList.Clear();
            SelectRemoteList.Clear();
            SelectLocalList.Clear();
        }

        //刷新初始化界面
        public void RefreshList(bool initTree= true)
        {
            This.Dispatcher.Invoke(() => {
                This.WaitEvent(() =>
                {
                    ClearUI();
                    WaitWindow.ChangeCallback(0, 10);
                    Thread.Sleep(100);
                    initlocalList();
                    WaitWindow.ChangeCallback(0, 30);
                    Thread.Sleep(100);
                    InitRemoteList();
                    WaitWindow.ChangeCallback(0, 50);
                    Thread.Sleep(100);
                    InitRelativeList();
                    WaitWindow.ChangeCallback(0, 100);
                    Thread.Sleep(100);
                });
            });
            Binging();

            AllRemoteFilesCheckBox.IsChecked = false;
            AllLocalFilesCheckBox.IsChecked = false;
            P2PConfig.This.SetFileDir(IP, DirRelative);
        }
        public void RefreshFileList()
        {
            NodeOperationInitList(DirRelative);
        }

        //点击文件树更新列表
        public void NodeOperationInitList(string dirRelative)
        {
            DirRelative = dirRelative;
            RefreshList(false);
        }

        //应用多车
        public void AppToHost(List<RobotOnlineInfo> Robotlist)
        {
            if (SelectLocalList.Count<=0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "请先选择需要发送的文件！", "错误");
                return;
            }

            if (Robotlist.Count <= 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "当前无在线车辆！", "错误");
                return;
            }

            SelectAGVWindow.Show(This, Robotlist, SelectLocalList,WorkMode.SendFile,DirRelative);
        }

        //文件Remote to Local
        private void OnClickedPullFiles(object sender, RoutedEventArgs e)
        {
            if (0 == SelectRemoteList.Count) return;

            String dir = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref dir);
            if (dir == null) return;

            string path = dir;

            int retval = -1;
            this.Dispatcher.Invoke(() => {
                this.WaitEvent(() => {
                    unsafe
                    {
                        retval = Tool.FileHandle.FTS.DownLoadFile(EP, SelectRemoteList, path);
                    }
                }, true, true);
            });

            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "下载文件失败！", "错误");
                return;
            }

            RefreshList();
            SelectRemoteList.Clear();
            AllRemoteFilesCheckBox.IsChecked = false;
            dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = false);
        }
        //文件Local to Remote
        private void OnClickedPushFiles(object sender, RoutedEventArgs e)
        {
            if (0 == SelectLocalList.Count) return;

            String dir = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref dir);
            if (dir == null) return;

            int retval = Tool.FileHandle.FTS.PushFile(EP, SelectLocalList, DirRelative);

            if (retval < 0 && -2 == retval)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "发送文件失败，目标IP已有其他任务正在运行！", "错误");
                return;
            }

            if (retval < 0 && -1 == retval)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "发送文件失败！", "错误");
                return;
            }

            TipsMessageEx.ShowTips(Tool.MainWindow.This, "已经加入传输列表", 18, 5, Colors.Green);


            AllLocalFilesCheckBox.IsChecked = false;
            SelectLocalList.Clear();
            dcLocal.Collection.ToList().ForEach(p => p.IsSelectedLocal = false);
        }


        //删除车载文件
        private void OnClickDeleteFile(object sender, RoutedEventArgs e)
        {
            List<TransFilesInfo> tmpList = new List<TransFilesInfo>();
            CheckSelFileList(ref tmpList);

            MessageBoxResult Result  = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "是否删除此文件？", "提示", MessageBoxButton.YesNo);
            if (Result == MessageBoxResult.No) return;

            int retval = -1;
            this.Dispatcher.Invoke(() => {
                this.WaitEvent(() => {
                    unsafe
                    {
                        retval = Tool.FileHandle.FTS.DeleteFile(EP, tmpList);
                    }
                }, true, true);
            });

            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "删除文件失败", "错误");
                RefreshList();
                dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = false);
                return;
            }

            RefreshList();
            dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = false);
            TipsMessageEx.ShowTips(Tool.MainWindow.This, "删除文件成功", 18, 5, Colors.Green);
        }

        //AGV车载文件下载到本地
        private void OnClickDownLoadFile(object sender, RoutedEventArgs e)
        {
            List<TransFilesInfo> tmpList = new List<TransFilesInfo>();
            CheckSelFileList(ref tmpList);

            if (0 == tmpList.Count) return;


            System.Windows.Forms.FolderBrowserDialog folder = new System.Windows.Forms.FolderBrowserDialog();
            folder.ShowDialog();

            if (folder.SelectedPath == string.Empty) return;

            int retval = -1;
            this.Dispatcher.Invoke(() => {
                this.WaitEvent(() => {
                    unsafe
                    {
                        retval = Tool.FileHandle.FTS.DownLoadFile(EP, tmpList, folder.SelectedPath);
                    }
                }, true, true);
            });
            
            if(retval<0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "下载文件失败！", "错误");
                return;
            }

            NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "下载文件完成", "提示");
            SelectRemoteList.Clear();
            dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = false);
            AllRemoteFilesCheckBox.IsChecked = false;
        }

        //选择列表文件
        private void OnSelectFiles(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            FileDetailInfo obj = (FileDetailInfo)cb.DataContext;

            TransFilesInfo info = new TransFilesInfo();

            if (true == cb.IsChecked)
            {
                if ("LocalFilesCheckBox" == cb.Name)
                {
                    info.Path = obj.LocalDir;
                    info.Type = obj.IsDir ? 0 : 1;
                    SelectLocalList.Add(info);
                }
                else if ("RemoteFilesCheckBox" == cb.Name)
                {
                    info.Path = obj.RemoteDir;
                    info.Type = obj.IsDir ? 0 : 1;
                    SelectRemoteList.Add(info);
                }
            }
            else
            {
                if ("LocalFilesCheckBox" == cb.Name)
                {
                    info.Path =  obj.LocalDir;
                    info.Type = obj.IsDir ? 0 : 1;
                    SelectLocalList.Remove(SelectLocalList.Where(p => p.Path == obj.LocalDir).FirstOrDefault());
                }
                else if ("RemoteFilesCheckBox" == cb.Name)
                {
                    info.Path = obj.RemoteDir;
                    info.Type = obj.IsDir ? 0 : 1;
                    SelectRemoteList.Remove(SelectRemoteList.Where(p => p.Path == obj.RemoteDir).FirstOrDefault());
                }
                    
            }
        }

        //全选列表文件
        private void OnSelectAllFiles(object sender, RoutedEventArgs e)
        {
            CheckBox cb = sender as CheckBox;
            if (cb.IsChecked == true)
            {
                if ("AllLocalFilesCheckBox" == cb.Name)
                {
                    List<FileDetailInfo> List = dcLocal.Collection.ToList();
                    foreach (var obj in List)
                    {
                        if ("" == obj.FileName) continue;

                        TransFilesInfo info = new TransFilesInfo();
                        info.Path = obj.LocalDir;
                        info.Type = obj.IsDir ? 0 : 1;
                        SelectLocalList.Add(info);
                    }
                    dcLocal.Collection.ToList().ForEach(p => p.IsSelectedLocal = true);
                }
                else if ("AllRemoteFilesCheckBox" == cb.Name)
                {
                    List<FileDetailInfo> List = dcRemote.Collection.ToList();
                    foreach(var obj in List)
                    {
                        if ("" == obj.FileName) continue;

                        TransFilesInfo info = new TransFilesInfo();
                        info.Path = obj.RemoteDir;
                        info.Type = obj.IsDir ? 0 : 1;
                        SelectRemoteList.Add(info);
                    }  
                    dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = true);
                }
            }
            else
            {
                if ("AllLocalFilesCheckBox" == cb.Name)
                {
                    SelectLocalList.Clear();
                    dcLocal.Collection.ToList().ForEach(p => p.IsSelectedLocal = false);
                }
                else if ("AllRemoteFilesCheckBox" == cb.Name)
                {
                    SelectRemoteList.Clear();
                    dcRemote.Collection.ToList().ForEach(p => p.IsSelectedRemote = false);
                }
            }
        }

        //双击列表文件夹
        private void OnDoubleClickFileList(object sender, MouseButtonEventArgs e)
        {
            ListView list = sender as ListView;
            if (null == list) return;

            if (null == PointItem) return;

            FileDetailInfo item = ((FileDetailInfo)PointItem);

            if (item.IsDir)
            {
                string strRelativeDir = "";
                if ("RemoteListView" == list.Name)
                {
                    string splitstr = IP;
                    if (null == item.RemoteDir) return; 
                    int pos = item.RemoteDir.IndexOf(IP);
                    strRelativeDir = item.RemoteDir;
                   
                }
                else if("LocalListView"== list.Name)
                {
                    if (null == item.LocalDir) return;
                    
                    strRelativeDir = item.LocalDir;
                }
                NodeOperationInitList(strRelativeDir);
            }
            else
            {
                if ("" == item.FileName) return;

                string pathRemote = "";
                if (item.RemoteDir != null)
                {
                    pathRemote = item.RemoteDir.Substring(1, item.RemoteDir.Length - 1);
                }


                String dir = null;
                Tool.Setting.ConfigRead.ReadLocalDir(ref dir);
                string pathLocal = "";
                if (item.LocalDir != null)
                {
                    pathLocal = dir + item.LocalDir.Substring(1, item.LocalDir.Length - 1);
                }
              

                if ("RemoteListView" == list.Name)
                {
                    

                    if (item.FileSize > 1024 * 1024)
                    {
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件大于1M，无法显示", "提示");
                        return;
                    }

                    TransFilesInfo info = new TransFilesInfo();
                    info.Path =  ((FileDetailInfo)PointItem).RemoteDir;
                    info.Type = ((FileDetailInfo)PointItem).IsDir ? 0 : 1;
                    List<TransFilesInfo> tmpList = new List<TransFilesInfo>();
                    tmpList.Add(info);

                    int retval = -1;
                    this.Dispatcher.Invoke(() => {
                        this.WaitEvent(() => {
                            unsafe
                            {
                               retval = Tool.FileHandle.FTS.DownLoadFile(EP, tmpList, BaseDir);
                            }
                        }, true, true);
                    });

                    if (retval < 0)
                    {
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "下载文件失败！", "错误");
                        return;
                    }

                }
                else if ("LocalListView" == list.Name)
                {
                    if (!CheckFileSize(pathLocal)) return;

                    if (item.FileSize > 1024 * 1024)
                    {
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件大于1M，无法显示", "提示");
                        return;
                    }

                    TransFilesInfo info = new TransFilesInfo();
                    info.Path = ((FileDetailInfo)PointItem).RemoteDir;
                    info.Type = ((FileDetailInfo)PointItem).IsDir ? 0 : 1;
                    List<TransFilesInfo> tmpList = new List<TransFilesInfo>();
                    tmpList.Add(info);

                    int retval = -1;
                    this.Dispatcher.Invoke(() => {
                        this.WaitEvent(() => {
                            unsafe
                            {
                                retval = Tool.FileHandle.FTS.DownLoadFile(EP, tmpList, BaseDir);
                            }
                        }, true, true);
                    });

                    if (retval < 0)
                    {
                        NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "下载文件失败！", "错误");
                        return;
                    }
                }
                WidCompareListEx.Show(Tool.MainWindow.This, EP, pathLocal, pathRemote, DirRelative);
            }
        }

        //检查文件大小
        private bool CheckFileSize(string path)
        {
            if ("" == path) return false;

            FileInfo fi = new FileInfo(path);
            if (fi.Length > (1024 * 1024))
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "文件大于1M，无法显示", "提示");
                return false;
            }
            return true;
        }

        //校验选择文件列表
        private void CheckSelFileList(ref List<TransFilesInfo> list)
        {
            FileDetailInfo item = (FileDetailInfo)PointItem;
            if (null == item)
            {
                list = SelectRemoteList;
            }

            else
            {
                if (item.IsSelectedRemote)
                {
                    list = SelectRemoteList;
                }
                else
                {
                    TransFilesInfo info = new TransFilesInfo();
                    info.Path = item.RemoteDir;
                    info.Type = item.IsDir ? 0 : 1;
                    list.Add(info);
                }
            }
        }

        //列表同步响应
        private void OnListScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            ListView list = sender as ListView;
            if (null == list) return;

            ListViewAutomationPeer lvapRemote = new ListViewAutomationPeer(list);
            var svaplvapRemote = lvapRemote.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
            var scrolllvapRemote = svaplvapRemote.Owner as ScrollViewer;

            ListViewAutomationPeer lvap = null;
            if ("LocalListView" == list.Name)
            {
               lvap = new ListViewAutomationPeer(RemoteListView);
            }
            else if ("RemoteListView" == list.Name)
            {
                lvap = new ListViewAutomationPeer(LocalListView);
               
            }

            var svap = lvap.GetPattern(PatternInterface.Scroll) as ScrollViewerAutomationPeer;
            var scroll = svap.Owner as ScrollViewer;
            scroll.ScrollToVerticalOffset(scrolllvapRemote.VerticalOffset);
            scroll.ScrollToHorizontalOffset(scrolllvapRemote.HorizontalOffset);
        }

        //文件目录后退
        public void NodeOperationStepBackFile()
        {
            if ("" == DirRelative) return;
            if (".\\" == DirRelative) return;

            string splitEndstr = "\\";
            int pos = DirRelative.LastIndexOf(splitEndstr);
            if (pos < 0) return;
            string dirRelative = DirRelative.Substring(0, pos);

            if (dirRelative == "") dirRelative = ".\\";
            if (dirRelative == ".") dirRelative = ".\\";
            DirRelative = dirRelative;

            RefreshList(false);
        }

        //获取车载列表右击焦点
        private void OnPreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            PointItem = GetElementFromPoint((ItemsControl)sender, e.GetPosition((ItemsControl)sender));
        }


        /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<ListView控件相关操作，无业务无关>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
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

        private static object GetFileInfoFromList(ListBox source, Point point)
        {
            UIElement element = source.InputHitTest(point) as UIElement;
            if (element == null) return null;
            
            object data = DependencyProperty.UnsetValue;
            while (data == DependencyProperty.UnsetValue)
            {
                data = source.ItemContainerGenerator.ItemFromContainer(element);
                if (data == DependencyProperty.UnsetValue)
                {
                    element = VisualTreeHelper.GetParent(element) as UIElement;
                }
                if (element == source)
                {
                    return null;
                }
            }
            if (data != DependencyProperty.UnsetValue) return data;

            return null;
        }

        public string GB2312ToUTF8(string text)
        {
            //声明字符集   
            System.Text.Encoding utf8, gb2312;
            //gb2312   
            gb2312 = System.Text.Encoding.GetEncoding("gb2312");
            //utf8   
            utf8 = System.Text.Encoding.GetEncoding("utf-8");
            byte[] gb;
            gb = gb2312.GetBytes(text);
            gb = System.Text.Encoding.Convert(gb2312, utf8, gb);
            //返回转换后的字符   
            return utf8.GetString(gb);
        }
    }
}

