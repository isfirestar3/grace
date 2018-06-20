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
using System.Xml.Linq;
using Tool.Net;
using Tool.CommView;
using System.ComponentModel;
using System.Collections.ObjectModel;


namespace Tool.P2PConfig
{
    public enum DifferType
    {
        Same =0,
        None=1,
        CRC32=2
    }

    public class TransFilesInfo
    {
        private string path;
        private int type;
        public string Path
        {
            get { return path; }
            set
            {
                path = value;
                
            }
        }

        public int Type
        {
            get { return type; }
            set
            {
                type = value;

            }
        }
    }

    public class FileDetailInfo : INotifyPropertyChanged
    {
        private int index;
        private string fileName;
        private string motifyLocalTime;
        private string motifyRemoteTime;
        private bool isDir;
        private bool isEnable;
        private string localCRC;
        private string remoteCRC;
        private bool isSelectedLocal = false;
        private bool isSelectedRemote = false;
        private string remoteDir;
        private string localDir;
        private DifferType localType = DifferType.Same;
        private DifferType remoteType = DifferType.Same;
        private UInt64 filesize;

        public int Index
        {
            get { return index; }
            set
            {
                index = value;
                OnChangedProperty("Index");
            }
        }

        public string FileName
        {
            get { return fileName; }
            set
            {
                fileName = value;
                OnChangedProperty("FileName");
            }
        }
        public string MotifyLocalTime
        {
            get { return motifyLocalTime; }
            set
            {
                motifyLocalTime = value;
                OnChangedProperty("MotifyLocalTime");
            }
        }

        public string MotifyRemoteTime
        {
            get { return motifyRemoteTime; }
            set
            {
                motifyRemoteTime = value;
                OnChangedProperty("MotifyRemoteTime");
            }
        }

        public bool IsDir
        {
            get { return isDir; }
            set
            {
                isDir = value;
                OnChangedProperty("IsDir");
            }
        }
        
        public string RemoteDir
        {
            get { return remoteDir; }
            set
            {
                remoteDir = value;
                OnChangedProperty("RemoteDir");
            }
        }

        public string LocalDir
        {
            get { return localDir; }
            set
            {
                localDir = value;
                OnChangedProperty("LocalDir");
            }
        }

        public bool IsEnable
        {
            get { return isEnable; }
            set
            {
                isEnable = value;
                OnChangedProperty("IsEnable");
            }
        }
        public string LocalCRC
        {
            get { return localCRC; }
            set
            {
                localCRC = value;
                OnChangedProperty("LocalCRC");
            }
        }

        public string RemoteCRC
        {
            get { return remoteCRC; }
            set
            {
                remoteCRC = value;
                OnChangedProperty("RemoteCRC");
            }
        }

        public bool IsSelectedLocal
        {
            get { return isSelectedLocal; }
            set
            {
                if (!isEnable) return;

                isSelectedLocal = value;
                OnChangedProperty("IsSelectedLocal");
            }
        }

        public bool IsSelectedRemote
        {
            get { return isSelectedRemote; }
            set
            {
                if (!isEnable) return;
                isSelectedRemote = value;
                OnChangedProperty("IsSelectedRemote");
            }
        }

        public DifferType LocalType
        {
            get { return localType; }
            set
            {
                localType = value;
                OnChangedProperty("LocalType");
            }
        }

        public DifferType RemoteType
        {
            get { return remoteType; }
            set
            {
                remoteType = value;
                OnChangedProperty("RemoteType");
            }
        }

        public UInt64 FileSize
        {
            get { return filesize; }
            set
            {
                filesize = value;
                OnChangedProperty("FileSize");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void OnChangedProperty(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public class FileDetailInfoRemoteCollect : INotifyPropertyChanged
    {
        private static FileDetailInfoRemoteCollect dataCollect = null;
        public static FileDetailInfoRemoteCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new FileDetailInfoRemoteCollect();

            return dataCollect;
        }


        private ObservableCollection<FileDetailInfo> collection = new ObservableCollection<FileDetailInfo>();
        public ObservableCollection<FileDetailInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
            }
        }

        public FileDetailInfoRemoteCollect()
        {
            
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void OnChangedProperty(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }

    public class FileDetailInfoLocalCollect : INotifyPropertyChanged
    {
        private static FileDetailInfoLocalCollect dataCollect = null;
        public static FileDetailInfoLocalCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new FileDetailInfoLocalCollect();

            return dataCollect;
        }


        private ObservableCollection<FileDetailInfo> collection = new ObservableCollection<FileDetailInfo>();
        public ObservableCollection<FileDetailInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
            }
        }

        public FileDetailInfoLocalCollect()
        {

        }

        public event PropertyChangedEventHandler PropertyChanged;
        public void OnChangedProperty(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
    }
}
