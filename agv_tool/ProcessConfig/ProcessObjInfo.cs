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
using Tool.XControl;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Threading;
using Tool.FileHandle;

namespace Tool.ProcessConfig
{
    public class ProcessObjInfo : INotifyPropertyChanged
    {
        private Int32 id;
        private string name;
        private string arg;
        private string path;
        private Int32 delaytime;

        public Int32 ID
        {
            get { return id; }
            set
            {
                id = value;
                OnChangedProperty("ID");
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
        
        public string Arg
        {
            get { return arg; }
            set
            {
                arg = value;
                OnChangedProperty("Arg");
            }
        }

        public string Path
        {
            get { return path; }
            set
            {
                path = value;
                OnChangedProperty("Path");
            }
        }

        public Int32 DelayTime
        {
            get { return delaytime; }
            set
            {
                delaytime = value;
                OnChangedProperty("DelayTime");
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

    public class ProcessInfoCollect : INotifyPropertyChanged
    {
        private static ProcessInfoCollect dataCollect = null;
        public static ProcessInfoCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new ProcessInfoCollect();

            return dataCollect;
        }


        private ObservableCollection<ProcessObjInfo> collection = new ObservableCollection<ProcessObjInfo>();

        public ObservableCollection<ProcessObjInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
            }
        }

        public ProcessInfoCollect()
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

        public void AddProcess(List<ProcessObjInfo> list)
        {
            Collection.Clear();
            foreach (var info in list)
            {
                Collection.Add(info);
            }
        }

        public bool AddProcess(ProcessObjInfo info)
        {
            Int32 id = 0;

            foreach (var obj in Collection.ToList())
            {
                if(obj.ID>id )
                {
                    id = obj.ID;
                }
            }

            if (id < 0) return false;

            info.ID = id+1;
            Collection.Add(info);
            return true;
        }


        public bool RemoveProcess(Int32 id)
        {
            Dictionary<Int32, ProcessObjInfo> varDict = Collection.ToList().ToDictionary(key => key.ID, value => value);

            if (!varDict.ContainsKey(id))
            {
                return false;
            }

            collection.Remove(varDict[id]);
            //重排ID
            Int32 i = 1;
            foreach (var obj in Collection.ToList())
            {
                obj.ID = i++;
            }

            return true;

        }

        public bool SortDownProcess(Int32 id)
        {
            Dictionary<Int32, ProcessObjInfo> varDict = Collection.ToList().ToDictionary(key => key.ID, value => value);
            if (!varDict.ContainsKey(id))
            {
                return false;
            }

            Int32 index = id;
            Int32 next_index = id + 1;

            if (next_index >= Collection.ToList().Count())
            {
                next_index = Collection.ToList().Count();
            }

            List<ProcessObjInfo> list = Collection.ToList();
            ProcessObjInfo a = list[index-1];
            ProcessObjInfo b = list[next_index - 1];
            Int32 tmp = a.ID;
            a.ID = b.ID;
            b.ID = tmp;

            list[index-1] =b;
            list[next_index - 1] = a;

            AddProcess(list);
            return true;
        }

        public bool SortUPProcess(Int32 id)
        {
            Dictionary<Int32, ProcessObjInfo> varDict = Collection.ToList().ToDictionary(key => key.ID, value => value);
            if (!varDict.ContainsKey(id))
            {
                return false;
            }

            Int32 index = id;
            Int32 pre_index = id- 1;

            if (pre_index <= 0)
            {
                pre_index = 1;
            }

            List<ProcessObjInfo> list = Collection.ToList();
            ProcessObjInfo a = list[index - 1];
            ProcessObjInfo b = list[pre_index - 1];
            Int32 tmp = a.ID;
            a.ID = b.ID;
            b.ID = tmp;

            list[index - 1] = b;
            list[pre_index - 1] = a;

            AddProcess(list);
            return true;
        }


    }
}
