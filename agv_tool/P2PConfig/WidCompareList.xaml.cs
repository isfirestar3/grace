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
using System.IO;
using Tool.CommView;
using Tool.Net;

namespace Tool.P2PConfig
{

    public partial class WidCompareList : Window
    {
        string DirRemote  =  "";
        string EP         =  "";
        bool   IsChanged  =  false;
        string SendFilePath = null;
        string ToDir = null;

        public WidCompareList(string ep,string pathLocal, string pathRemote,string toDir)
        {
            InitializeComponent();
            this.Owner = Tool.MainWindow.This;
            Left = 0;
            Top = 0;
            Width = Tool.MainWindow.This.Width;
            Height = Tool.MainWindow.This.Height;

            GridFile.Width = Tool.MainWindow.This.Width-200;
            GridFile.Height = Tool.MainWindow.This.Height-150;
            this.WindowStartupLocation = WindowStartupLocation.CenterOwner;

            InitLocalFile(pathLocal);
           
            string dir = System.AppDomain.CurrentDomain.BaseDirectory+ pathRemote;
            dir = dir.Replace("\\\\","\\");
            SendFilePath = dir;
            InitRemoteFile(dir);
            DirRemote = dir;
            EP = ep;
            ToDir = toDir;
            IsChanged = false;
        }

        private void InitLocalFile(string path)
        {
            string text="";
            if (File.Exists(path))
            {

                this.WaitEvent(() =>
                {
                    text = InitFile(path);
                });

                    if (null==text)
                {
                    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "读取标准文件错误", "提示");
                    return;
                }
                TBLocal.Text = text;
            }
        }

        private void InitRemoteFile(string path)
        {
            string text = "";
            if ('\\' == path[path.Count() - 1])
            {
                path = path.Substring(0, path.Count() - 1);
            }
            if (File.Exists(path))
            {
                this.WaitEvent(() =>
                {
                    text = InitFile(path);
                });

                if (null == text)
                {
                    NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "读取目标车载文件错误", "提示");
                    return;
                }
                TBRemote.Text = text;
            }
        }

        public string InitFile(string path)
        {
            string text = "";
            try
            {
                FileStream fs = new FileStream(path, FileMode.Open, FileAccess.ReadWrite);

                //int fsLen = (int)fs.Length;
                //byte[] heByte = new byte[fsLen];
                //int r = fs.Read(heByte, 0, heByte.Length);

                //string myStr = System.Text.Encoding.UTF8.GetString(heByte);

                StreamReader m_streamReader = new StreamReader(fs);                             
                m_streamReader.BaseStream.Seek(0, SeekOrigin.Begin);

                text = m_streamReader.ReadToEnd();
               
                m_streamReader.Close();
                fs.Close();
                
            }
            catch
            {
                return null;
            }

            return text;
        }

        public static void Show(FrameworkElement Owner, string ep, string pathLocal, string pathRemote,string toDir)
        {
            WidCompareList wid = new WidCompareList(ep,pathLocal, pathRemote,toDir);
            wid.ShowDialog();
            return;
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
            MessageBoxResult isSave = MessageBoxResult.No;
            if (IsChanged)
            {
                string str = string.Format("文件已修改,请确认是否保存？");
                isSave = NotifyMessageEx.ShowNotify(Tool.MainWindow.This, str, "提示", System.Windows.MessageBoxButton.YesNo);
                if (isSave == MessageBoxResult.Yes)
                {
                    SaveFile();
                }
            }

            this.Dispatcher.Invoke(() => { Close(); });
            Tool.P2PConfig.List2List.This.RefreshFileList();
        }

        private void OnLocalScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            ScrollViewer scroll = sender as ScrollViewer;
            if (null == scroll) return;

          
            ScrollRemote.ScrollToVerticalOffset(scroll.VerticalOffset);
            ScrollRemote.ScrollToHorizontalOffset(scroll.HorizontalOffset);
        }

        private void OnRemoteScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            ScrollViewer scroll = sender as ScrollViewer;
            if (null == scroll) return;

            ScrollLocal.ScrollToVerticalOffset(scroll.VerticalOffset);
            ScrollLocal.ScrollToHorizontalOffset(scroll.HorizontalOffset);
        }

        private void SaveFile()
        {
            if (null == DirRemote) return;
            if (Directory.Exists(DirRemote)) return;

            string text = TBRemote.Text.ToString();
            
            FileStream fs = new FileStream(DirRemote, FileMode.Create);

            byte[] data = System.Text.Encoding.Default.GetBytes(text);
            fs.Write(data, 0, data.Length);
            fs.Flush();
            fs.Close();

            List<TransFilesInfo> tmpList = new List<TransFilesInfo>();
            TransFilesInfo info = new TransFilesInfo();

            info.Path = SendFilePath;
            info.Type = 1;
            tmpList.Add(info);

            int retval = 0;

            string dir_path = ToDir + DirRemote.Substring(DirRemote.LastIndexOf("\\"));

            Tool.Proto.ProtoMSG protomsg = new Proto.ProtoMSG();
            protomsg.MSG.value_ = dir_path;
            byte[] b_msg = new byte[protomsg.length()];
            Int32 offset = 0;
            protomsg.serialize(b_msg, ref offset);

            this.WaitEvent(() =>
            {
                unsafe
                {
                    void* r_data = null;
                    int r_len = 0;
                    int net_id = Tool.RobotsHome.RobotInfoCollect.GetInstance().FindNetIdByfts(EP);

                    unsafe
                    {
                        fixed (byte* l_msg = b_msg)
                        {
                            retval = NetInterface.NetPostSyncPackage(net_id, (int)NetInterface.PKTTYPE.kAgvShellProto_ProtoBackFiles, l_msg, protomsg.length(), &r_data, &r_len);
                        }
                    }

                }
            }, true, true);

            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "备份失败！", "错误");
                return;
            }

            retval = Tool.FileHandle.FTS.SaveFile(EP, tmpList, ToDir);

            if (retval < 0 && -2 == retval)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "保存文件失败，目标IP已有其他任务正在运行！", "错误");
                return;
            }

            if (retval < 0 && -1 == retval)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "保存文件失败！", "错误");
                return;
            }
        }

        private void OnSaveFile(object sender, RoutedEventArgs e)
        {
            SaveFile();
            
            this.Dispatcher.Invoke(() => { Close(); });
            
            Tool.P2PConfig.List2List.This.RefreshFileList();
        }

        private void OnTextChanged(object sender, TextChangedEventArgs e)
        {
            IsChanged = true;
        }

        private void OnCancle(object sender, RoutedEventArgs e)
        {
            this.Dispatcher.Invoke(() => { Close(); });
        }
    }

    public static class WidCompareListEx
    {
        public static void Show(this FrameworkElement Owner,string ep,string pathLocal,string pathRemote,string toDir)
        {
            WidCompareList.Show(Owner, ep,pathLocal, pathRemote, toDir);
        }
    }
}

  