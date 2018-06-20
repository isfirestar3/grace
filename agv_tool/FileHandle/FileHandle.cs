using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media;
using Tool.Net;
using Tool.XControl;
using Tool.CommView;
using System.IO;
using System.Windows;
using Tool.P2PConfig;
using Tool.Proto;

namespace Tool.FileHandle
{
    public static class FTS
    {
        public enum SendObject
        {
            Local=1,
            Remote=0
        }

       static public int PushFiles(List<string> listEP, List<TransFilesInfo> listDir,string toDir)
        {
            if (0 == listDir.Count) return -1;


            ProtoSendFiles info = new ProtoSendFiles();

            String BaseDir = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref BaseDir);
            if (BaseDir == null) return -1;

            foreach (var dirs in listDir)
            {
                FileDir obj = new FileDir();
                obj.FileName.value_ = BaseDir + dirs.Path.Substring(1, dirs.Path.Length - 1);
                info.FileList.Add(obj);
            }

            info.ToDir.value_ = toDir;

            byte[] b_data = null;
            int len = 0;
            b_data = new byte[info.length()];
            if (!info.serialize(b_data, ref len)) return -1;
            
            string epList = null;
            foreach (string ep in listEP)
            {
                if (ep == null) continue;
                epList += ep + "|";
            }

            unsafe
            {
                fixed (byte* data_stream = b_data)
                {
                    return NetInterface.NetPushFilesToRemote(epList, data_stream, len);
                }
            }
        }

        static public int PushFile(string ep, List<TransFilesInfo> listFiles,string dir)
        {
            if (0 == listFiles.Count) return -1;
            ProtoSendFiles info = new ProtoSendFiles();

            String BaseDir = null;
            Tool.Setting.ConfigRead.ReadLocalDir(ref BaseDir);
            if (dir == null) return-1;

            foreach (var dirs in listFiles)
            {
                FileDir obj = new FileDir();

                obj.FileName.value_ = BaseDir + dirs.Path.Substring(1, dirs.Path.Length-1);
                info.FileList.Add(obj);
            }
            info.ToDir.value_ = dir;

            byte[] b_data = null;
            int len = 0;
            b_data = new byte[info.length()];
            if (!info.serialize(b_data, ref len)) return -1;
            
            unsafe
            {
                fixed (byte* data_stream = b_data)
                {
                    return NetInterface.NetPushFilesToRemote(ep, data_stream, len);
                }
            }
        }

        static public int SaveFile(string ep, List<TransFilesInfo> listFiles, string dir)
        {
            if (0 == listFiles.Count) return -1;
            ProtoSendFiles info = new ProtoSendFiles();

            foreach (var dirs in listFiles)
            {
                FileDir obj = new FileDir();

                obj.FileName.value_ =dirs.Path;
                info.FileList.Add(obj);
            }
            info.ToDir.value_ = dir;

            byte[] b_data = null;
            int len = 0;
            b_data = new byte[info.length()];
            if (!info.serialize(b_data, ref len)) return -1;

            unsafe
            {
                fixed (byte* data_stream = b_data)
                {
                    return NetInterface.NetPushFilesToRemote(ep, data_stream, len);
                }
            }
        }

        static public int DownLoadFile(string ep, List<TransFilesInfo> listFiles,string downloadDir)
        {
            if (0 == listFiles.Count) return -1;

            ProtoDownLoadFiles info = new ProtoDownLoadFiles();
            
            foreach(var obj in listFiles)
            {
                DownLoadFiles file = new DownLoadFiles();
                file.FilePath.value_ = obj.Path;
                file.FileType.value_ = obj.Type;
                info.FileListList.Add(file);
            }
            
            byte[] b_data = null;
            int len = 0;
            b_data = new byte[info.length()];
            if (!info.serialize(b_data, ref len)) return -1;

            unsafe
            {
                fixed (byte* data_stream = b_data)
                {
                    return NetInterface.NetPullFilesFromRemote(ep, data_stream, len, downloadDir); 
                }
            }
        }

        static public int DeleteFile(string ep, List<TransFilesInfo> listDir)
        {
            if (0 == listDir.Count)
            {
                return -1;
            }

            ProtoDeleteFiles info = new ProtoDeleteFiles();

            foreach (var obj in listDir)
            {
                DeleteFiles file = new DeleteFiles();
                file.FilePath.value_ = obj.Path;
                file.FileType.value_ = obj.Type;
                info.FileListList.Add(file);
            }

            byte[] b_data = null;
            int len = 0;
            b_data = new byte[info.length()];
            if (!info.serialize(b_data, ref len)) return -1;

            unsafe
            {
                fixed (byte* data_stream = b_data)
                {
                    return NetInterface.NetDeleteRemoteFiles(ep, data_stream, len);
                }
            }
        }

        static public int UpdateFiles(string ep, List<string> listDir)
        {
            if (NetInterface.NetRegistPullCallback(WaitWindow.ChangeCallback) < 0) return -1;

            string dirList = null;
            foreach (string dir in listDir)
            {
                if (dir == null) continue;
                dirList += dir + "|";
            }

            return NetInterface.NetUpdateRemoteFileList(ep, dirList); ;
        }
    }
}
