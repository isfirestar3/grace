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
using Tool.Net;
using System.IO;
using Tool.CommView;
using Tool.RobotsHome;

namespace Tool.TransferPanel
{
    public partial class FileTransferPage : Page
    {
        static public FileTransferPage This = null;

        public struct TaskGroupInfo
        {
            public UInt64 TaskId;
            [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst = 24)]
            public string TargetEp;
            public Int32 TaskStatus;
            public UInt32 TaskStep;
        }

        public FileTransferPage()
        {
            InitializeComponent();
            This = this;
            NetInterface.NetRegistPushCallback(PushFilesCallBack);
        }

        //文件传输任务组回调函数
        unsafe public static NetInterface.MsgCallbackPushFiles PushFilesCallBack = new NetInterface.MsgCallbackPushFiles(
            (void* str_info, int cb) =>
            {
                FileTransferPage.This.Dispatcher.Invoke(() =>
                {
                    if (null == This) return;

                    Dictionary<string, Tool.RobotsHome.RobotInfo> robotinfo = new Dictionary<string, Tool.RobotsHome.RobotInfo>();
                    RobotInfoCollect.GetInstance().GetRobotinfoFTS(ref robotinfo);

                    List<TaskDetailinfo> TaskDetailinfoList = new List<TaskDetailinfo>();
                    List<TaskDetailinfo> TaskSuccessDetailinfoList = new List<TaskDetailinfo>();

                    byte[] byte_stream = new byte[cb];
                    using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)str_info, cb))
                    {
                        ms.Read(byte_stream, 0, byte_stream.Length);
                    }
                    int length = System.Runtime.InteropServices.Marshal.SizeOf(typeof(TaskGroupInfo));
                    int count = cb / length;
                    TaskGroupInfo info;
                    UInt32 totalStep = 0;
                    for (int i = 0; i < count; i++)
                    {
                        info.TaskId = BitConverter.ToUInt64(byte_stream, i * length);
                        info.TargetEp = System.Text.Encoding.Default.GetString(byte_stream, i * length + 8, 24).Trim();
                        info.TaskStatus = BitConverter.ToInt32(byte_stream, i * length + 32);
                        info.TaskStep = BitConverter.ToUInt32(byte_stream, i * length + 36);
                        TaskDetailinfo Detailinfo = new TaskDetailinfo();

                        int pos = info.TargetEp.IndexOf("\0");
                        if (pos > 0)
                        {
                            info.TargetEp = info.TargetEp.Substring(0, pos);
                        }

                        if (robotinfo.ContainsKey(info.TargetEp))
                        {
                            Detailinfo.Id = robotinfo[info.TargetEp].Id;
                            Detailinfo.Name = robotinfo[info.TargetEp].Id.ToString();
                        }

                        Detailinfo.TargetEp = info.TargetEp;
                        Detailinfo.TaskStatus = info.TaskStatus;
                        Detailinfo.TaskId = info.TaskId;
                        Detailinfo.TaskStep = info.TaskStep;
                        totalStep += info.TaskStep;


                        if (info.TaskStatus > 0)
                        {
                            TaskSuccessDetailinfoList.Add(Detailinfo);
                        }
                        else
                        {
                            TaskDetailinfoList.Add(Detailinfo);
                        }
                    }

                    This.TotolProgressBar.Value = (totalStep * 100 / (100 * count));
                    string str = string.Format("{0}%", This.TotolProgressBar.Value);
                    This.TotolProgressText.Text = str;
                    This.ProgressBar.Value = (totalStep * 100 / (100 * count));
                    This.ProgressText.Text = str;

                    This.TransformingListView.ItemsSource = TaskDetailinfoList;
                    This.TransformingListView.Items.Refresh();

                    This.TransforSuccessListView.ItemsSource = TaskSuccessDetailinfoList;
                    This.TransformingListView.Items.Refresh();
                    return;
                });
            }
        );

        //传输详情
        private void OnMouseCheckFileDetail(object sender, MouseButtonEventArgs e)
        {
            TextBlock tb = sender as TextBlock;
            List<TaskFileDetailInfo> list = new List<TaskFileDetailInfo>();
            string str = tb.Tag.ToString();
            int retval = NetInterface.NetGetPushFilesDetailInfo(Convert.ToUInt64(str), ref list);
            if (retval < 0)
            {
                NotifyMessageEx.ShowNotify(Tool.MainWindow.This, "获取文件详情失败！", "错误");
                return;
            }
            TransFromDetailInfoWid.Show(this, Convert.ToUInt64(str), list);
        }
    }
}
