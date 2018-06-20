using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;
using ProtoDefine;
using Tool.CommView;
using Tool.TransferPanel;

namespace Tool.Net
{
    
    public class NetInterface
    {
        public enum PKTTYPE
        {
            kAgvShellProto_Process_List = 0,
            kAgvShellProto_Status       = 1,
            kAgvShellProto_System_Time = 2,
            kAgvShellProto_UpdateNTPServer=3,              
            kAgvShellProto_RunScript =4,
            kAgvShellProto_ProcessDetail = 5,
            kAgvShellProto_UpdateProcessDetail = 6,
            kAgvShellProto_VCUEnable = 7,
            kAgvShellProtoProto_GetDHCPList = 8,
            kAgvShellProto_NotifyDHCP =9,
            kAgvShellProto_ModifyMutex =10,
            kAgvShellProto_FileMutexStatus =47,
            kAgvShellProto_ShellVersionInfo = 55,
            kAgvShellProto_ProtoShellProgress = 11,
            kAgvShellProto_ProtoBackFiles = 12,
            kAgvShellProto_ConfigVersionInfo = 60,
        }

        public delegate void fn_link_closed_t(Int32 RobotId);
        public delegate void fn_link_connected_t(Int32 RobotId,Int32 Status,string HostName);
        public delegate void fn_list_dir_t(Int32 status, Int32 step);
        public delegate void fn_init_agv_t(Int32 step,Int32 status,string ip);

        public struct task_file_detail_info
        {
            [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst = 512)]
            public string file_name_;
            [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst = 32)]
            public string file_modify_time_;
            public UInt64 file_size_;
            public Int32  file_status_;
            public UInt32 file_step_;
        }

        //1. 初始化网络
        [DllImport(@"operation_net.dll", EntryPoint = "init_net", CallingConvention = CallingConvention.StdCall)]
        public extern static Int32  init(fn_link_closed_t closeEvt, fn_link_connected_t connectEvt, string ep);

        //2. 连接网络
        [DllImport(@"operation_net.dll", EntryPoint = "connect_host", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 Connect(Int32 robotId, string ep);
        
        //3. 断开网络
        [DllImport(@"operation_net.dll", EntryPoint = "disconnect", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 Disconnect(Int32 robotId);

        //4. 拉取对端文件列表
        [DllImport(@"operation_net.dll", EntryPoint = "get_listdir", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 NetGetRomoteListDir(string ep,string dir, void** data_ptr, int* cb);

        //5. 发送文件到对端
        [DllImport(@"operation_net.dll", EntryPoint = "push_files_to_remote", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 NetPushFilesToRemote(string ep, void *dirList, Int32 len);

        //6. 下载对端文件
        [DllImport(@"operation_net.dll", EntryPoint = "pull_files_from_remote", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 NetPullFilesFromRemote(string ep, void* dirList, Int32 len, string savePath);

        //7. 获取文件CRC
        [DllImport(@"operation_net.dll", EntryPoint = "get_file_crc", CallingConvention = CallingConvention.StdCall)]
        unsafe public extern static Int32 NetCRC32(string dir, ref UInt32 CRC);

        //8. 拉取对端文件回调
        [DllImport(@"operation_net.dll", EntryPoint = "regist_pull_callback", CallingConvention = CallingConvention.StdCall)]
        public extern static UInt32 NetRegistPullCallback(WaitWindow.ProgressCallback callback);

        //9. 任务组回调
        public unsafe delegate void MsgCallbackPushFiles(void* str_push_info, int cb);
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "regist_push_callback", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetRegistPushCallback(MsgCallbackPushFiles callback);

        //10. 删除对端文件
        [DllImport(@"operation_net.dll", EntryPoint = "delete_remote_files", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static Int32 NetDeleteRemoteFiles(string ep, void* dirList, Int32 len);

        //11. 获取任务组详情
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "get_push_files_detail", CallingConvention = CallingConvention.StdCall)]
        unsafe extern static int GetPushFilesDetail(UInt64 task_id, void** data_ptr, ref int cb);
        public unsafe static int NetGetPushFilesDetailInfo(UInt64 task_id, ref List<TaskFileDetailInfo> list)
        {
            void* data_ptr;
            int file_len = 0;
            GetPushFilesDetail(task_id, &data_ptr, ref file_len);
            int detail_length = System.Runtime.InteropServices.Marshal.SizeOf(typeof(task_file_detail_info));
            int detail_count = file_len / detail_length;
            byte[] file_stream = new byte[file_len];
            using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)data_ptr, file_len))
            {
                ms.Read(file_stream, 0, file_stream.Length);
            }

            for (int j = 0; j < detail_count; j++)
            {
                TaskFileDetailInfo info = new TaskFileDetailInfo();

                info.ID = j + 1;
                info.FileName = System.Text.Encoding.Default.GetString(file_stream, j * detail_length, 512).Trim();
                int pos = info.FileName.IndexOf("\0");
                if (pos > 0)
                {
                    info.FileName = info.FileName.Substring(0, pos);
                }

                info.FileModifyTime = System.Text.Encoding.Default.GetString(file_stream, j * detail_length + 512, 32).Trim();
                pos = info.FileModifyTime.IndexOf("\0");
                if (pos > 0)
                {
                    info.FileModifyTime = info.FileModifyTime.Substring(0, pos);
                }

                info.FileSize = BitConverter.ToUInt64(file_stream, j * detail_length + 544);
                info.FileStatus = BitConverter.ToInt32(file_stream, j * detail_length + 552);
                info.FileStep = BitConverter.ToUInt32(file_stream, j * detail_length + 556);
                info.IsFileSelected = false;
                list.Add(info);
            }

            if (data_ptr != null)
            {
                FreeDetail(data_ptr);
            }
            return 0;
        }

        //12. 释放资源
        [DllImport(@"operation_net.dll", EntryPoint = "free_detail", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static int FreeDetail(void* stream);

        //13. 重发文件
        [DllImport(@"operation_net.dll", EntryPoint = "repeat_push_files", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static int NetRepeatFilesToRemote(UInt64 taskid, string dirList, Int32 len);

        //14. 初始化车载文件
        [DllImport(@"operation_net.dll", EntryPoint = "restore_factory_setting", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static int NetInitFactorySetting(string ep,fn_init_agv_t evt);
       
        //15.更新车载文件
        [DllImport(@"operation_net.dll", EntryPoint = "pull_files_from_remote", CallingConvention = CallingConvention.StdCall)]
        public unsafe extern static  Int32 NetUpdateRemoteFileList(string ep, string dirList);

        //16. 管理进程
        [DllImport(@"operation_net.dll", EntryPoint = "post_cmd_agv_process", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPostCodeAgvProcess(string robotId, string process,Tool.RobotsHome.CmdAgvShell cmd);

        //17. 设置块大小
        [DllImport(@"operation_net.dll", EntryPoint = "set_block_size", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern void NetSetBlockSize(Tool.Setting.TransferBlockSize size);

        //18. 取消同步拉文件
        [DllImport(@"operation_net.dll", EntryPoint = "cancel_pull_opreate", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern void CancelPullOpreate();

        //19. 固件升级回调函数
        public unsafe delegate void MsgCallbackUpdateBin(string ep,int type, string msg);
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "regist_m_core_info_callback", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetRegistCallbackUpdateBin(MsgCallbackUpdateBin callback);

        //20. 获取bin文件信息
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "get_m_camera_info_request", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetGetBinDetailInfo(string netId, int type, string target_ep, int node_id, int can_bus);

        //21. 更新bin文件
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "push_m_camera_file", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetUpdateBinFiles(string fts, int type,string path,string target_ep);

        //22. 下载bin文件
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "pull_m_camera_file", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetDownloadBinFiles(string netId, int type,int len, string path,string target_ep);

        //23. 重启
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "restart_m_core", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetResetCore(string netId, int type, string target_ep);

        //24. 固件操作回调
        public unsafe delegate void MsgCallbackTransStep(string ep, int type, int step);
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "regist_m_core_transfer_callback", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetRegistCallbackTransStep(MsgCallbackTransStep callback);

        //25. AGVSHEll 升级
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "update_agv_shell", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetUpdateAgvShell(string netId, string path);

        //26. 通用网络包
        public unsafe delegate void NetMsgCallBack(string robot_ip, int type,int error,void*msgAck,int cb);
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "regist_netowrk_callback", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetRegsiterMsgCallback(NetMsgCallBack callback);

        //27. 获取进程表
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "get_agv_process_table", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern void NetQueryProcessesTableCallback();

        //28. 获取当前路劲下的文件列表
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "get_current_catalog", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetQueryCurrentCatalog(string ft_endpoint, string input_catalog, void ** data_ptr, int* cb);

        //29. 异步发包
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "post_async_package", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPostAsyncPackage(int robot_id, int type, void* str, int cb);

        //30. 同步发包
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "post_sync_package", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPostSyncPackage(int robot_id, int type, void* str, int cb, void** _ack_msg, int* _ack_len);

        //31. 是否打开DHCP功能
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "is_open_dhcp", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int OpenDhcp(bool status);

        //32. 同步发包 无Robot_id
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "post_sync_without_id", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPostSyncPackageWithoutId(int type, void* str, int cb, void** _ack_msg, int* _ack_len);

        //33. 更新自主驱动
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "push_can_bin_file", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPushCanBinFile(string fts, int node_id, int node_type,string path, string target_ep);

        //34. 备份
        [DllImport(@"operation_net.dll", CharSet = CharSet.Ansi, EntryPoint = "post_backup_file", CallingConvention = CallingConvention.StdCall)]
        public unsafe static extern int NetPostBackupFile(void* str, int cb);

        public static void WriteLog(string documentName, string msg)
        {
            string errorLogFilePath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Log");
            if (!System.IO.Directory.Exists(errorLogFilePath))
            {
                System.IO.Directory.CreateDirectory(errorLogFilePath);
            }
            string logFile = System.IO.Path.Combine(errorLogFilePath, documentName + "@" + DateTime.Today.ToString("yyyy-MM-dd") + ".txt");
            bool writeBaseInfo = System.IO.File.Exists(logFile);
            StreamWriter swLogFile = new StreamWriter(logFile, true, Encoding.Unicode);
            swLogFile.WriteLine(DateTime.Now.ToString("HH:mm:ss") + "\t" + msg);
            swLogFile.Close();
            swLogFile.Dispose();
        }

        //通用网络包回调函数
        unsafe public static NetMsgCallBack NetWorkCallback = new NetMsgCallBack(
            (string robot_ip, int type, int error, void* msgAck, int cb) =>
            {
               if (null == msgAck) return;
                byte[] ack_msg  = new byte[cb];
                using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)msgAck, cb))
                {
                    ms.Read(ack_msg, 0, ack_msg.Length); 
                }
                
                switch ((PKTTYPE)type)
                {
                    case PKTTYPE.kAgvShellProto_Process_List:
                        Tool.Proto.Unpackage<Proto.ProtoProcessTable> table = new Proto.Unpackage<Proto.ProtoProcessTable>(ack_msg);
                        if(table.build()) 
                        {
                            Proto.ProtoProcessTable statusList = table.ob;
                            ProcessManager.ProcessManager.This.AddListColumn(robot_ip, statusList.ProcessTableList);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_Status:
                        Tool.Proto.Unpackage<Proto.ProtoProcessStatus> status = new Proto.Unpackage<Proto.ProtoProcessStatus>(ack_msg);
                        if (status.build())
                        {
                            Proto.ProtoProcessStatus statusList = status.ob;
                            ProcessManager.ProcessManager.This.UpdateProcessStatus(robot_ip, statusList.ProcessStatusList);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_System_Time:
                        Tool.Proto.Unpackage<Proto.ProtoSystemTime> time = new Proto.Unpackage<Proto.ProtoSystemTime>(ack_msg);
                        if (time.build())
                        {
                            Proto.ProtoSystemTime curTime = time.ob;
                            RobotsHome.RobotInfoCollect.GetInstance().UpdateSystemTime(robot_ip, curTime);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_ProcessDetail:
                        Proto.Unpackage<Proto.ProcessDetail> info = new Proto.Unpackage<Proto.ProcessDetail>(ack_msg);
                        if (info.build())
                        {
                            Proto.ProcessDetail detailInfo = info.ob;
                        }
                        break;

                    case PKTTYPE.kAgvShellProto_NotifyDHCP:
                        Tool.CarHouse.CartHousePage.This.UpdateRobotList();
                        break;

                    case PKTTYPE.kAgvShellProto_FileMutexStatus:
                        Proto.Unpackage<Proto.ProtoMuteStatus> IsLock= new Proto.Unpackage<Proto.ProtoMuteStatus>(ack_msg);
                        if (IsLock.build())
                        {
                            Proto.ProtoMuteStatus fileStatus = IsLock.ob;
                            RobotsHome.RobotInfoCollect.GetInstance().UpdateFileLockStatus(robot_ip, fileStatus.IsLock.value_);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_ShellVersionInfo:
                        Proto.Unpackage<Proto.ProtoMSG> MSG = new Proto.Unpackage<Proto.ProtoMSG>(ack_msg);
                        if (MSG.build())
                        {
                            Proto.ProtoMSG obj = MSG.ob;
                            RobotsHome.RobotInfoCollect.GetInstance().UpdateShellVersionInfo(robot_ip, obj.MSG.value_);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_ProtoShellProgress:
                        Proto.Unpackage<Proto.ProtoMSG> fininsded = new Proto.Unpackage<Proto.ProtoMSG>(ack_msg);
                        if (fininsded.build())
                        {
                            Proto.ProtoMSG obj = fininsded.ob;
                            RobotsHome.RobotInfoCollect.GetInstance().UpdateShellPercent(robot_ip, Convert.ToInt32(obj.MSG.value_));
                            WriteLog("Shell", "percent:"+obj.MSG.value_);
                        }
                        break;
                    case PKTTYPE.kAgvShellProto_ConfigVersionInfo:
                        Proto.Unpackage<Proto.ProtoMSG> ver = new Proto.Unpackage<Proto.ProtoMSG>(ack_msg);
                        if (ver.build())
                        {
                            Proto.ProtoMSG obj = ver.ob;
                            RobotsHome.RobotInfoCollect.GetInstance().UpdateConfigVersionInfo(robot_ip, obj.MSG.value_);
                        }
                        break;
                    default:
                        break;
                }
            }
        );
    }
}
