using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Collections.ObjectModel;


namespace Tool.TransferPanel
{
    public class TaskFileDetailInfo : INotifyPropertyChanged
    {
        private int id;
        private string fileName;
        private string fileModifyTime;
        private UInt64 fileSize;
        private Int32  fileStatus;
        private UInt32 fileStep;
        private bool   isFileSelected;

        public int ID
        {
            get { return id; }
            set
            {
                id = value;
                OnChangedProperty("ID");
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

        public string FileModifyTime
        {
            get { return fileModifyTime; }
            set
            {
                fileModifyTime = value;
                OnChangedProperty("FileModifyTime");
            }
        }

        public UInt64 FileSize
        {
            get { return fileSize; }
            set
            {
                fileSize = value;
                OnChangedProperty("FileSize");
            }
        }

        public Int32 FileStatus
        {
            get { return fileStatus; }
            set
            {
                fileStatus = value;
                OnChangedProperty("FileStatus");
            }
        }

        public UInt32 FileStep
        {
            get { return fileStep; }
            set
            {
                fileStep = value;
                OnChangedProperty("FileStep");
            }
        }

        public bool IsFileSelected
        {
            get { return isFileSelected; }
            set
            {
                isFileSelected = value;
                OnChangedProperty("IsFileSelected");
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

    public class TaskFileDetailInfoCollect : INotifyPropertyChanged
    {
        private static TaskFileDetailInfoCollect dataCollect = null;
        public static TaskFileDetailInfoCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new TaskFileDetailInfoCollect();

            return dataCollect;
        }


        private ObservableCollection<TaskFileDetailInfo> collection = new ObservableCollection<TaskFileDetailInfo>();
        public ObservableCollection<TaskFileDetailInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
            }
        }

        public TaskFileDetailInfoCollect()
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
