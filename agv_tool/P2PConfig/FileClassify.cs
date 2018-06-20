using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Tool.CommView;
using System.Windows;
using Tool.Net;

namespace Tool.P2PConfig
{
    class FileClassify
    { 
        public  static void InitFileClassify( List<FileDetailInfo> standard,List<FileDetailInfo> remote, 
                                   ref List<FileDetailInfo> standardOut, ref List<FileDetailInfo> remoteOut)
        {
            int MaxCount = (standard.Count > remote.Count ? standard.Count : remote.Count);
            Dictionary<string, FileDetailInfo> MapTotalFileInfo = new Dictionary<string, FileDetailInfo>();

            Dictionary<string, FileDetailInfo>  dictStandard = standard.ToDictionary(key => key.FileName, value => value);
            Dictionary<string, FileDetailInfo > dictRemote = remote.ToDictionary(key => key.FileName, value => value);

            //求远端和本地文件信息的全集
            for (int i = 0; i < MaxCount; i++)
            {
                if (i < standard.Count)
                {
                    if (!MapTotalFileInfo.ContainsKey(standard[i].FileName))
                    {
                        MapTotalFileInfo[standard[i].FileName] = standard[i];
                    }
                }
                if (i < remote.Count)
                {
                    if (!MapTotalFileInfo.ContainsKey(remote[i].FileName))
                    {
                        MapTotalFileInfo[remote[i].FileName] = remote[i];
                    }
                }
            }

            //全集跟各自的文件信息对比，不存在的FileName=""
            Dictionary<string, FileDetailInfo> tmpStandardFileInfo = new Dictionary<string, FileDetailInfo>(MapTotalFileInfo);
            Dictionary<string, FileDetailInfo> tmpRemoteFileInfo = new Dictionary<string, FileDetailInfo>(MapTotalFileInfo);

            foreach (string filename in MapTotalFileInfo.Keys)
            {
                FileDetailInfo info = MapTotalFileInfo[filename];
                if (!dictStandard.ContainsKey(info.FileName))
                {
                    FileDetailInfo InvalidInfo = new FileDetailInfo();
                    InvalidInfo.FileName = "";
                    InvalidInfo.IsDir = dictRemote[info.FileName].IsDir;
                    InvalidInfo.IsEnable = false;
                    InvalidInfo.MotifyLocalTime = "";
                    InvalidInfo.LocalCRC = "";
                    tmpStandardFileInfo[info.FileName] = InvalidInfo;
                }

                if (!dictRemote.ContainsKey(info.FileName))
                {
                    FileDetailInfo InvalidInfo = new FileDetailInfo();
                    InvalidInfo.FileName = "";
                    InvalidInfo.IsDir = dictStandard[info.FileName].IsDir;
                    InvalidInfo.IsEnable = false;
                    InvalidInfo.MotifyRemoteTime = "";
                    InvalidInfo.RemoteCRC = "";
                    tmpRemoteFileInfo[info.FileName] = InvalidInfo;
                }
            }


            //还原目录
            foreach (string filename in tmpStandardFileInfo.Keys)
            {
                if (filename == "") continue;

                if (dictStandard.ContainsKey(filename))
                {
                    tmpStandardFileInfo[filename].LocalDir = dictStandard[filename].LocalDir;
                    tmpStandardFileInfo[filename].LocalCRC = dictStandard[filename].LocalCRC;
                    tmpStandardFileInfo[filename].MotifyLocalTime = dictStandard[filename].MotifyLocalTime;
                }
            }

            foreach (string filename in tmpRemoteFileInfo.Keys)
            {
                if (filename == "") continue;

                if (dictRemote.ContainsKey(filename))
                {
                    tmpRemoteFileInfo[filename].RemoteDir = dictRemote[filename].RemoteDir;
                    tmpRemoteFileInfo[filename].RemoteCRC = dictRemote[filename].RemoteCRC;
                    tmpRemoteFileInfo[filename].MotifyRemoteTime = dictRemote[filename].MotifyRemoteTime;
                    tmpRemoteFileInfo[filename].IsEnable = dictRemote[filename].IsEnable;
                }
            }

            //比较差异性,个数一致
            foreach (string filename in tmpStandardFileInfo.Keys)
            {
                if (!tmpRemoteFileInfo.ContainsKey(filename)) continue;

                if (("" ==tmpRemoteFileInfo[filename].FileName &&  "" != tmpStandardFileInfo[filename].FileName) 
                    || ("" != tmpRemoteFileInfo[filename].FileName && "" == tmpStandardFileInfo[filename].FileName))
                {
                    tmpRemoteFileInfo[filename].RemoteType = DifferType.None;
                    tmpStandardFileInfo[filename].LocalType = DifferType.None;
                    continue;
                }


                if ("" != tmpRemoteFileInfo[filename].FileName && "" != tmpStandardFileInfo[filename].FileName)
                {
                    if (tmpStandardFileInfo[filename].IsDir)
                    {
                        //UInt32 localCRC32 = 0;
                        //UInt32 remoteCRC32 = 0;
                        //NetInterface.NetCRC32(tmpStandardFileInfo[filename].LocalDir, ref localCRC32);
                        //NetInterface.NetCRC32(tmpRemoteFileInfo[filename].RemoteDir, ref remoteCRC32);

                        if(tmpStandardFileInfo[filename].LocalCRC != tmpRemoteFileInfo[filename].RemoteCRC)
                        {
                            tmpRemoteFileInfo[filename].RemoteType = DifferType.CRC32;
                            tmpRemoteFileInfo[filename].LocalType = DifferType.CRC32;
                        }
                    }

                    else
                    {
                        if (tmpStandardFileInfo[filename].LocalCRC != tmpRemoteFileInfo[filename].RemoteCRC)
                        {
                            tmpRemoteFileInfo[filename].RemoteType = DifferType.CRC32;
                            tmpRemoteFileInfo[filename].LocalType = DifferType.CRC32;
                        }
                    }
                }
            }

            //重新排列
            standardOut = tmpStandardFileInfo.Values.ToList<FileDetailInfo>();
            remoteOut = tmpRemoteFileInfo.Values.ToList<FileDetailInfo>();
              
        }

        
    }
}
