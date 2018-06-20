using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Tool.CommView;
using System.Windows;
using Tool.Net;

namespace Tool.FileHandle
{
    
    public static class FileOperator
    {
        //提取文件目录，不关心文件
        public static void CopyDir(string fromDir, string toDir)
        {
            if (!Directory.Exists(fromDir))
                return;

            if (!Directory.Exists(toDir))
            {
                Directory.CreateDirectory(toDir);
            }

            string[] fromDirs = Directory.GetDirectories(fromDir);
            foreach (string fromDirName in fromDirs)
            {
                string dirName = Path.GetFileName(fromDirName);
                string toDirName = Path.Combine(toDir, dirName);
                CopyDir(fromDirName, toDirName);
            }
        }

        //拷贝目录到目录
        public static void CopyDirFiles(string fromDir, string toDir,bool IsOverWrite)
        {
            if (!Directory.Exists(fromDir))
                return;

            if (!Directory.Exists(toDir))
            {
                Directory.CreateDirectory(toDir);
            }

            string[] files = Directory.GetFiles(fromDir);
            foreach (string formFileName in files)
            {
                string fileName = Path.GetFileName(formFileName);
                string toFileName = Path.Combine(toDir, fileName);
                if (!File.Exists(toFileName))
                {
                    File.Copy(formFileName, toFileName);
                }
                else
                {
                    File.Copy(formFileName, toFileName, IsOverWrite);
                }
            }

            string[] fromDirs = Directory.GetDirectories(fromDir);
            foreach (string fromDirName in fromDirs)
            {
                string dirName = Path.GetFileName(fromDirName);
                string toDirName = Path.Combine(toDir, dirName);
                CopyDirFiles(fromDirName, toDirName, IsOverWrite);
            }

        }

        //拷贝文件到文件
        public static void CopyFiles(string fromfile, string tofile, bool IsOverWrite)
        {
            if (!File.Exists(fromfile))
                return;


            //必须考虑断层
            string FileName = System.IO.Path.GetFileName(tofile);
            int pos = tofile.IndexOf(FileName);
            string str = tofile.Substring(0,pos);

            if (!Directory.Exists(str))
            {
                Directory.CreateDirectory(str);
            }

            if (!File.Exists(tofile))
            {
                File.Copy(fromfile, tofile);
            }
            else
            {
                File.Copy(fromfile, tofile, IsOverWrite);
            }
        }

        public static void DelectDir(string srcPath)
        {
            try
            {
                DirectoryInfo dir = new DirectoryInfo(srcPath);
                FileSystemInfo[] fileinfo = dir.GetFileSystemInfos();
                foreach (FileSystemInfo i in fileinfo)
                {
                    if (i is DirectoryInfo)            
                    {
                        DirectoryInfo subdir = new DirectoryInfo(i.FullName);
                        subdir.Delete(true);          
                    }
                    else
                    {
                        File.Delete(i.FullName);     
                    }
                }
            }
            catch (Exception)
            {
                throw;
            }
        }


        //剔除目录中重复的文件夹中的文件
        public  static bool DifferDirFiles(string fromDir, string toDir, ref List<string> list,ref bool isDifferent)
        {
            if (!Directory.Exists(fromDir))
                return isDifferent;

           
            string[] files = Directory.GetFiles(fromDir);
            foreach (string fromFileName in files)
            {
                string fileName = Path.GetFileName(fromFileName);
                string toFileName = Path.Combine(toDir, fileName);
                if (File.Exists(toFileName))
                {
                    isDifferent = true;
                    continue;
                }
                list.Add(fromFileName);
            }

            string[] fromDirs = Directory.GetDirectories(fromDir);
            foreach (string fromDirName in fromDirs)
            {
                string dirName = Path.GetFileName(fromDirName);
                string toDirName = Path.Combine(toDir, dirName);
                DifferDirFiles(fromDirName, toDirName, ref list, ref isDifferent);
            }

            return isDifferent;
        }

        public static bool DifferFiles(string fromfile, string tofile, ref List<string> list)
        {
            bool retval = false;
            if (!File.Exists(fromfile))
                return retval;

            if (File.Exists(tofile))
            {
                return true;
            }
            list.Add(fromfile);
            return retval;
        }

        public static bool GetDifferFileList(List<string> fileList, ref List<string> differlist,string IP, Tool.P2PConfig.FileTYPE to)
        {
            bool hasSame = false;
            foreach (string path in fileList)
            {
                string pathSrc = path;

                String dir = null;
                string pathCopy = null;
                string dirSub = null;
                int posStart = 0;
                int posEnd = 0;
                switch (to)
                {
                    case Tool.P2PConfig.FileTYPE.Local:
                        
                        Tool.Setting.ConfigRead.ReadLocalDir(ref dir);
                        if (dir == null) return false;
                        posStart = pathSrc.IndexOf(IP);
                        posEnd = pathSrc.Substring(posStart).IndexOf("\\");
                        dirSub = pathSrc.Substring(posStart + posEnd);
                        pathCopy = dir + dirSub;
                        
                        break;
                    case Tool.P2PConfig.FileTYPE.Remote:
                        //Tool.Setting.ConfigRead.ReadRemoteDir(ref dir);
                        //if (dir == null) return false;
                        //posStart = pathSrc.IndexOf("standard");
                        //posEnd = pathSrc.Substring(posStart).IndexOf("\\");
                        //dirSub = pathSrc.Substring(posStart + posEnd);
                        //pathCopy = dir + "\\" + IP + dirSub;
                        break;
                }
                
                bool isSame = false;
                FileInfo info = new FileInfo(pathSrc);
                FileAttributes attr = info.Attributes;
                if (attr == FileAttributes.Directory)
                {
                    DifferDirFiles(pathSrc, pathCopy, ref differlist,ref isSame);
                }
                else
                {
                    isSame = DifferFiles(pathSrc, pathCopy, ref differlist);
                }

                if (!hasSame)
                {
                    hasSame = isSame;
                }
            }
            return hasSame;
        }
    }
}
