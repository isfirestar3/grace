using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace Tool.SelectAGV
{
    public class RobotOnlineInfo : INotifyPropertyChanged
    {
        private Int32 id;
        private string name;
        private string fts;
        private string ep;
        private string hostName;
        private bool isSelected = false;
        public int Id
        {
            get { return id; }
            set
            {
                id = value;
                OnChangedProperty("Id");
            }
        }
        public string Fts
        {
            get { return fts; }
            set
            {
                fts = value;
                OnChangedProperty("Fts");
            }
        }

        public string EP
        {
            get { return ep; }
            set
            {
                ep = value;
                OnChangedProperty("EP");
            }
        }
        public string Name
        {
            get { return name; }
            set
            {
                name = value;
                OnChangedProperty("Name");
            }
        }

        public string HostName
        {
            get { return hostName; }
            set
            {
                hostName = value;
                OnChangedProperty("HostName");
            }
        }

        public bool IsSelected
        {
            get { return isSelected; }
            set
            {
                isSelected = value;
                OnChangedProperty("IsSelected");
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

    public class RobotOnlineInfoCollect : INotifyPropertyChanged
    {
        private static RobotOnlineInfoCollect dataCollect = null;
        public static RobotOnlineInfoCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new RobotOnlineInfoCollect();

            return dataCollect;
        }


        private ObservableCollection<RobotOnlineInfo> collection = new ObservableCollection<RobotOnlineInfo>();

        public ObservableCollection<RobotOnlineInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
            }
        }

        public RobotOnlineInfoCollect()
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
