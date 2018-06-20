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
using System.IO;
using Tool.SelectAGV;



namespace Tool.RobotsHome
{
    public class RobotInfo : INotifyPropertyChanged
    {
        private Int32 id;
        private string hostName;
        private string endPoint;
        private Int32 netId;
        private bool netState=false;
        private string fts;
        private bool isSelected = false;
        private string macAddr; 

        //固件升级
        private string modelName;
        private string modelVersion;
        private string modelCPU;
        private Int32  progressRate;
        private bool   processState = false;
        private Int32  binStatus;
        private bool   update = false;         //是否正在升级
        private bool   isSelectedBin = false;

        //进程状态
        private Int32  processIdNo1 = -1;
        private string processNameNo1 = "未知";
        private Int32  processStatusNo1 = -1;

        private Int32  processIdNo2 = -2;
        private string processNameNo2 = "未知";
        private Int32  processStatusNo2 = -1;

        private Int32  processIdNo3 = -3;
        private string processNameNo3 = "未知";
        private Int32  processStatusNo3 = -1;

        private Int32  processIdNo4 = -4;
        private string processNameNo4 = "未知";
        private Int32  processStatusNo4 = -1;

        private Int32  processIdNo5 = -5;
        private string processNameNo5 = "未知";
        private Int32  processStatusNo5 = -1;

        private bool   isSelectedProcess = false;


        private string systemTime = "";
        private Int32  vcuStatus = 0;
        private Int32  isFileLock = 0;

        private string versionInfo = "";
        private Int32 shellProcessPrec = 0;

        private string configInfo = "";
        public int Id
        {
            get { return id; }
            set
            {
                id = value;
                OnChangedProperty("Id");
            }
        }
        public string EndPoint
        {
            get { return endPoint; }
            set
            {
                endPoint = value;
                OnChangedProperty("EndPoint");
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
        public Int32 NetId
        {
            get { return netId; }
            set
            {
                netId = value;
                OnChangedProperty("NetId");
            }
        }
        public bool NetState
        {
            get { return netState; }
            set
            {
                netState = value;
                OnChangedProperty("NetState");
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
        public bool IsSelected
        {
            get { return isSelected; }
            set
            {
                isSelected = value;
                OnChangedProperty("IsSelected");
            }
        }
        public string ModelName
        {
            get { return modelName; }
            set
            {
                modelName = value;
                OnChangedProperty("ModelName");
            }
        }
        public string ModelVersion
        {
            get { return modelVersion; }
            set
            {
                modelVersion = value;
                OnChangedProperty("ModelVersion");
            }
        }
        public string ModelCPU
        {
            get { return modelCPU; }
            set
            {
                modelCPU = value;
                OnChangedProperty("ModelCPU");
            }
        }
        public Int32 ProgressRate
        {
            get { return progressRate; }
            set
            {
                progressRate = value;
                OnChangedProperty("ProgressRate");
            }
        }
        public Int32 BinStatus
        {
            get { return binStatus; }
            set
            {
                binStatus = value;
                OnChangedProperty("BinStatus");
            }
        }
        public bool BinUpdate
        {
            get { return update;}
            set
            {
                update = value;
                OnChangedProperty("Update");
            }
        }
        public bool IsSelectedBin
        {
            get { return isSelectedBin; }
            set
            {
                isSelectedBin = value;
                OnChangedProperty("IsSelectedBin");
            }
        }
        public bool ProcessState
        {
            get {
                 return processState;
            }
            set
            {
                processState = value;
                OnChangedProperty("ProcessState");
            }
        }
        public Int32 ProcessIdNo1
        {
            get { return processIdNo1; }
            set
            {
                processIdNo1 = value;
                OnChangedProperty("ProcessIdNo1");
            }
        }
        public string ProcessNameNo1
        {
            get { return processNameNo1; }
            set
            {
                processNameNo1 = value;
                OnChangedProperty("ProcessNameNo1");
            }
        }
        public Int32 ProcessStatusNo1
        {
            get { return processStatusNo1; }
            set
            {
                processStatusNo1 = value;
                OnChangedProperty("ProcessStatusNo1");
            }
        }
        public Int32 ProcessIdNo2
        {
            get{return processIdNo2;}
            set
            {
                processIdNo2 = value;
                OnChangedProperty("ProcessIdNo2");
            }
        }
        public string ProcessNameNo2
        {
            get{return processNameNo2;}
            set
            {
                processNameNo2 = value;
                OnChangedProperty("ProcessNameNo2");
            }
        }
        public Int32 ProcessStatusNo2
        {
            get{return processStatusNo2;}
            set
            {
                processStatusNo2 = value;
                OnChangedProperty("ProcessStatusNo2");
            }
        }
        public Int32 ProcessIdNo3
        {
            get{return processIdNo3;}
            set
            {
                processIdNo3 = value;
                OnChangedProperty("ProcessIdNo3");
            }
        }
        public string ProcessNameNo3
        {
            get{return processNameNo3;}
            set
            {
                processNameNo3 = value;
                OnChangedProperty("ProcessNameNo3");
            }
        }
        public Int32 ProcessStatusNo3
        {
            get{return processStatusNo3;}
            set
            {
                processStatusNo3 = value;
                OnChangedProperty("ProcessStatusNo3");
            }
        }
        public Int32 ProcessIdNo4
        {
            get{return processIdNo4;}
            set
            {
                processIdNo4 = value;
                OnChangedProperty("ProcessIdNo4");
            }
        }
        public string ProcessNameNo4
        {
            get{return processNameNo4;}
            set
            {
                processNameNo4 = value;
                OnChangedProperty("ProcessNameNo4");
            }
        }
        public Int32 ProcessStatusNo4
        {
            get { return processStatusNo4; }
           
            set
            {
                processStatusNo4 = value;
                OnChangedProperty("ProcessStatusNo4");
            }
        }
        public Int32 ProcessIdNo5
        {
            get { return processIdNo5; }
           
            set
            {
                processIdNo5 = value;
                OnChangedProperty("ProcessIdNo5");
            }
        }
        public string ProcessNameNo5
        {
            get { return processNameNo5; }
            
            set
            {
                processNameNo5 = value;
                OnChangedProperty("ProcessNameNo5");
            }
        }
        public Int32 ProcessStatusNo5
        {
            get { return processStatusNo5; }
           
            set
            {
                processStatusNo5 = value;
                OnChangedProperty("ProcessStatusNo5");
            }
        }

        
        public bool IsSelectedProcess
        {
            get { return isSelectedProcess; }
           
            set
            {
                isSelectedProcess = value;
                OnChangedProperty("IsSelectedProcess");
            }
        }

        public string SystemTime
        {
            get { return systemTime; }
            set
            {
                systemTime = value;
                OnChangedProperty("SystemTime");
            }
        }

        public Int32 VCUStatus
        {
            get { return vcuStatus; }
            set
            {
                vcuStatus = value;
                OnChangedProperty("VCUStatus");
            }
        }

        public Int32 IsFileLock
        {
            get { return isFileLock; }
            set
            {
                isFileLock = value;
                OnChangedProperty("IsFileLock");
            }
        }

        public string VersionInfo
        {
            get { return versionInfo; }
          
            set
            {
                versionInfo = value;
                OnChangedProperty("VersionInfo");
            }
        }

        public Int32 ShellProcessPerc
        {
            get { return shellProcessPrec; }
            set
            {
                shellProcessPrec = value;
                OnChangedProperty("ShellProcessPerc");
            }
        }

        public string MacAddr
        {
            get { return macAddr; }

            set
            {
                macAddr = value;
                OnChangedProperty("MacAddr");
            }
        }

        public string ConfigInfo
        {
            get { return configInfo;}

            set
            {
                configInfo = value;
                OnChangedProperty("ConfigInfo");
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

    public class RobotInfoCollect : INotifyPropertyChanged
    {
        private static RobotInfoCollect dataCollect = null;
        public static RobotInfoCollect GetInstance()
        {
            if (dataCollect == null)
                dataCollect = new RobotInfoCollect();

            return dataCollect;
        }

        Int32 ItemCount=0;
        private ObservableCollection<RobotInfo> collection = new ObservableCollection<RobotInfo>();
        private List<string> InitFileList = new List<string>();
        public ObservableCollection<RobotInfo> Collection
        {
            get { return collection; }
            set
            {
                collection = value;
                OnChangedProperty("Collection");
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

        
        public RobotInfoCollect()
        {
            ReadXML();
        }

        //初始化XML文件
        public void ReadXML()
        {
            try
            {
                XDocument XDoc = XDocument.Load("RobotInfo.xml");
                XElement rootNode = XDoc.Element("RobotInfo");
                Dictionary<string, RobotInfo> varDict = new Dictionary<string, RobotInfo>();
                foreach (XElement node in rootNode.Elements("item"))
                {
                    RobotInfo info = new RobotInfo();
                    info.Id = Convert.ToInt32(node.Attribute("name").Value);
                    info.EndPoint = node.Attribute("ip").Value + ":" + node.Attribute("port").Value;
                    info.NetId = -(ItemCount++);
                    info.NetState = false;
                    info.Fts = node.Attribute("fts").Value;
                    if (varDict.ContainsKey(info.EndPoint))
                    {
                        continue;
                    }
                    varDict.Add(info.EndPoint, info);
                    Collection.Add(info);
                }
            }
            catch (Exception)
            {
                
            }
        }
        public void FromDHCP()
        {

            Int32 retval = -1;
            Proto.ProtoRobotList list = new Proto.ProtoRobotList();

            unsafe
            {
                void* r_data = null;
                int r_len = 0;
                retval = NetInterface.NetPostSyncPackageWithoutId((int)NetInterface.PKTTYPE.kAgvShellProtoProto_GetDHCPList, null, 0, &r_data, &r_len);
                if (retval < 0 || null == r_data) return;

                byte[] ack_msg = null;

                ack_msg = new byte[r_len];
                using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)r_data, r_len))
                {
                    ms.Read(ack_msg, 0, ack_msg.Length);
                }

                Tool.Proto.Unpackage<Proto.ProtoRobotList> info = new Proto.Unpackage<Proto.ProtoRobotList>(ack_msg);
                if (info.build())
                {
                    list = info.ob;
                }
                NetInterface.FreeDetail(r_data);
            }

            Dictionary<string, RobotInfo> varDict = new Dictionary<string, RobotInfo>();
            Collection.Clear();
            foreach (Proto.ProtoRobotInfo node in list.RobotList)
            {
                RobotInfo info = new RobotInfo();
                info.Id = node.CarID.value_;
                info.EndPoint = node.IPV4.value_ + ":" + node.AGVPort.value_;
                info.NetId = -(ItemCount++);
                info.NetState = false;
                info.Fts = node.IPV4.value_ + ":" + node.FTSPort.value_;
                info.MacAddr = node.Mac.value_;
                info.Id = node.CarID.value_;
                if (varDict.ContainsKey(info.EndPoint))
                {
                    continue;
                }
                varDict.Add(info.EndPoint, info);
                Collection.Add(info);
            }
        }
        public void UpdateDHCP()
        {
            Int32 retval = -1;
            Proto.ProtoRobotList list = new Proto.ProtoRobotList();

            unsafe
            {
                void* r_data = null;
                int r_len = 0;
                retval = NetInterface.NetPostSyncPackageWithoutId((int)NetInterface.PKTTYPE.kAgvShellProtoProto_GetDHCPList, null, 0, &r_data, &r_len);
                if (retval < 0 || null == r_data) return;

                byte[] ack_msg = null;

                ack_msg = new byte[r_len];
                using (UnmanagedMemoryStream ms = new UnmanagedMemoryStream((byte*)r_data, r_len))
                {
                    ms.Read(ack_msg, 0, ack_msg.Length);
                }

                Tool.Proto.Unpackage<Proto.ProtoRobotList> info = new Proto.Unpackage<Proto.ProtoRobotList>(ack_msg);
                if (info.build())
                {
                    list = info.ob;
                }
                NetInterface.FreeDetail(r_data);
            }

            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            foreach (Proto.ProtoRobotInfo node in list.RobotList)
            {
                if (varDict.ContainsKey(node.IPV4.value_ + ":" + node.AGVPort.value_)) continue ;
               
                RobotInfo info = new RobotInfo();
                info.Id = node.CarID.value_;
                info.EndPoint = node.IPV4.value_ + ":" + node.AGVPort.value_;
                info.NetId = -(ItemCount++);
                info.NetState = false;
                info.Fts = node.IPV4.value_ + ":" + node.FTSPort.value_;
                info.MacAddr = node.Mac.value_;
                info.Id = node.CarID.value_;
                varDict.Add(info.EndPoint, info);
                Collection.Add(info);
            }

            Dictionary<string, RobotInfo> dict = new Dictionary<string, RobotInfo>();
            foreach (Proto.ProtoRobotInfo node in list.RobotList)
            {
                RobotInfo info = new RobotInfo();
                info.Id = node.CarID.value_;
                info.EndPoint = node.IPV4.value_ + ":" + node.AGVPort.value_;
                info.NetId = -(ItemCount++);
                info.NetState = false;
                info.Fts = node.IPV4.value_ + ":" + node.FTSPort.value_;
                info.MacAddr = node.Mac.value_;
                info.Id = node.CarID.value_;
                dict[node.IPV4.value_ + ":" + node.AGVPort.value_] = info;
            }
            
            foreach (RobotInfo node in Collection.ToList())
            {
                if (!dict.ContainsKey(node.EndPoint))
                {
                    Collection.Remove(varDict[node.EndPoint]);
                }
            }
        }
        public void CleanRobotObj()
        {
            ItemCount = 0;
            Collection.Clear();
        }

        //查询是否连接
        public bool IsConnectedHost(string ep)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);
            if (!varDict.ContainsKey(ep))
            {
                return false;
            }

            return varDict[ep].NetState;
        }

        //查找网络对象ID
        public Int32 FindNetId(string ep)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);
            if (!varDict.ContainsKey(ep))
            {
                return -1;
            }
            return varDict[ep].NetId;
        }

        public Int32 FindNetIdByfts(string ep)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.Fts, value => value);
            if (!varDict.ContainsKey(ep))
            {
                return -1;
            }
            return varDict[ep].NetId;
        }

        public Int32 FindCarId(string ep)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);
            if (!varDict.ContainsKey(ep))
            {
                return -1;
            }
            return varDict[ep].Id;
        }

        public string FindNetEP(int netId)
        {
            Dictionary<int, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.NetId, value => value);
            if (!varDict.ContainsKey(netId))
            {
                return null;
            }
            return varDict[netId].EndPoint;
        }

        public Int32 FindBinStatusByFTS(string fts_ep)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.Fts, value => value);
            if (!varDict.ContainsKey(fts_ep))
            {
                return -1;
            }
            return varDict[fts_ep].BinStatus;
        }

        //更新网络对象ID
        public void UpdateNetId(string ep, Int32 netid)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return;
            }
            varDict[ep].NetId = netid;
        }

        //更新网络ID
        public void UpdateNetStatus(Int32 netid, bool connect)
        { 
            Dictionary<int, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.NetId, value => value);

            if (!varDict.ContainsKey(netid))
            {
                return;
            }
            varDict[netid].NetState = connect;
        }

        //更新MAC
        public void UpdateMac(Int32 netid, string mac)
        {
            Dictionary<int, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.NetId, value => value);

            if (!varDict.ContainsKey(netid))
            {
                return;
            }
            varDict[netid].MacAddr = mac;
        }

        //更新主机名
        public void Reset(Int32 netid)
        {
            Dictionary<int, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.NetId, value => value);

            if (!varDict.ContainsKey(netid))
            {
                return;
            }
            varDict[netid].SystemTime = "";
            varDict[netid].VersionInfo = "";
        }

        //过滤在线车辆
        public  void FliterRobotOnline(ref List<RobotOnlineInfo> list)
        {
            for (int i = 0; i < Collection.ToList().Count; i++)
            {
                if (Collection.ToList()[i].NetState)
                {
                    RobotOnlineInfo info = new RobotOnlineInfo();
                    info.Id = Collection.ToList()[i].Id;
                    info.Fts = Collection.ToList()[i].Fts;
                    info.EP = Collection.ToList()[i].EndPoint;
                    info.HostName = Collection.ToList()[i].HostName;
                    list.Add(info);
                }
            }
        }

        public void FliterRobotOnline (ref List<string> list)
        {
            for (int i = 0; i < Collection.ToList().Count; i++)
            {
                if (Collection.ToList()[i].NetState)
                {
                    list.Add(Collection.ToList()[i].EndPoint);
                }
            }
        }

        //获取机器人信息 key：fts端口
        public void GetRobotinfoFTS(ref Dictionary<string, RobotInfo> dictRobotInfo)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.Fts, value => value);
            dictRobotInfo = varDict;
        }

        //获取机器人信息 key：AGV Shell
        public void GetRobotinfoShell(ref Dictionary<string, RobotInfo> dictRobotInfo)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);
            dictRobotInfo = varDict;
        }

        ////////////////////////////以下函数操作管理车辆XML文件的增删改//////////////////////////////////
        //添加车辆
        public bool AppendRobotInfo(List<RobotInfo> list)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            XDocument XDoc = XDocument.Load("RobotInfo.xml");
            XElement rootNode = XDoc.Element("RobotInfo");
            XNode xLastNode = rootNode.LastNode;

            bool retval = false;
            foreach (RobotInfo info in list)
            {
                if (varDict.ContainsKey(info.EndPoint)) continue;

                info.NetId = -(ItemCount++);
                Collection.Add(info);
                try
                {
                    XElement xEle = new XElement("item");
                    XAttribute xAttr = new XAttribute("name", info.Id.ToString());
                    xEle.Add(xAttr);
                    var endpoint = info.EndPoint.ToString().Split(':');
                    if (endpoint.Length != 2) continue;
                  
                    xAttr = new XAttribute("ip", endpoint[0]);
                    xEle.Add(xAttr);
                    xAttr = new XAttribute("port", endpoint[1]);
                    xEle.Add(xAttr);
                    xAttr = new XAttribute("fts", info.Fts.ToString());
                    xEle.Add(xAttr);
                   
                    if (xLastNode!=null)
                    {
                        xLastNode.AddAfterSelf(xEle);
                        xLastNode = xEle;
                    }
                    else
                    {
                        rootNode.AddFirst(xEle);
                        xLastNode = rootNode;
                    }
                    retval = true;
                }
                catch (Exception)
                {
                    return false;
                }
            }
            XDoc.Save("RobotInfo.xml");
            return retval;
        }

        //更新车辆
        public bool UpdateRobotInfo(RobotInfo editInfo, RobotInfo updateInfo)
        {
            bool retval = false;
            

            retval = UpdateXML(editInfo, updateInfo);
            if (!retval)
            {
                return false;
            }

            if (!UpdateUIList(editInfo, updateInfo))
            {
                return false;
            }

            return true;
        }

        //删除车辆
        public bool RemoveRobotInfo(List<string> list)
        {
            bool retval = false;
            retval = RemoveXmlObj(list);
            retval = RemoveUIList(list);
            return retval;
        }
        
        private bool UpdateXML(RobotInfo editInfo, RobotInfo updateInfo)
        {
            bool retval = false;
            try
            {
                XDocument XDoc = XDocument.Load("RobotInfo.xml");
                XElement rootNode = XDoc.Element("RobotInfo");
                foreach (XElement node in rootNode.Elements("item"))
                {
                    string str = node.Attribute("ip").Value + ":" + node.Attribute("port").Value;
                    if (str == editInfo.EndPoint)
                    {
                        node.Attribute("name").Value = updateInfo.Id.ToString();
                        var ep = updateInfo.EndPoint.Split(':');
                        if (ep.Length != 2)
                        {
                            retval = false;
                            break;
                        }
                        node.Attribute("ip").Value = ep[0];
                        node.Attribute("port").Value = ep[1];
                        var ftsEP = node.Attribute("fts").Value.Split(':');
                        if (ep.Length == 2)
                        {
                            node.Attribute("fts").Value = ep[0] + ":"+ ftsEP[1];
                        }
                       
                        retval = true;
                        break;
                    }
                }
                XDoc.Save("RobotInfo.xml");
            }
            catch (Exception)
            {
                retval = false;
            }
            return retval;
        }

        private bool UpdateUIList(RobotInfo editInfo, RobotInfo updateInfo)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (varDict.ContainsKey(updateInfo.EndPoint))
            {
                if(updateInfo.EndPoint!= editInfo.EndPoint)
                {
                    return false;
                }
            }

            if (!varDict.ContainsKey(editInfo.EndPoint))
            {
                return false;
            }
           
            varDict[editInfo.EndPoint].Id = Convert.ToInt32(updateInfo.Id);
            var ep = updateInfo.EndPoint.Split(':');
            var ftsep = updateInfo.EndPoint.Split(':');
            if (ep.Length == 2 && ftsep.Length == 2)
            {
                varDict[editInfo.EndPoint].Fts = ep[0] + ":"+"4411";
            }
            
            varDict[editInfo.EndPoint].EndPoint = updateInfo.EndPoint;
            return true;
        }

        private bool RemoveXmlObj(List<string> list)
        {
            try
            {
                XDocument XDoc = XDocument.Load("RobotInfo.xml");
                XElement rootNode = XDoc.Element("RobotInfo");
                foreach(string ep in list)
                {
                    foreach (XElement node in rootNode.Elements("item"))
                    {
                        string str = node.Attribute("ip").Value + ":" + node.Attribute("port").Value;
                        if(ep==str)
                        {
                            node.Remove();
                        }
                        
                    }
                }
                XDoc.Save("RobotInfo.xml");
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }

        private bool RemoveUIList(List<string> list)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);
            foreach (string ep in list)
            {
                if (!varDict.ContainsKey(ep))
                {
                    return false;
                }
                collection.Remove(varDict[ep]);
            }
            return true;
        }

        //清除固件升级信息
        public void ClearBinInfo()
        {
            List<RobotInfo> info = Collection.ToList();

            for (int i = 0; i < info.Count; i++)
            {
                info[i].ModelCPU = "";
                info[i].ModelName = "";
                info[i].ModelVersion = "";
            }
        }

        //更新进程映射表
        public bool UpdateProcessTable(string ep, List<Tool.Proto.ProcessTable> list)
        {
            List<RobotInfo>info =  Collection.ToList();

            for(int i = 0; i < info.Count; i++)
            {
                for(int j = 0; j < list.Count;j++)
                {
                    if (j > 5) break;
                    if (0 == j)
                    {
                        info[i].ProcessIdNo1 = list[j].ProcessId.value_;
                        info[i].ProcessNameNo1 = list[j].ProcessName.value_;
                        info[i].ProcessStatusNo1 = -1;
                        continue;
                    }

                    if (1 == j)
                    {
                        info[i].ProcessIdNo2 = list[j].ProcessId.value_;
                        info[i].ProcessNameNo2 = list[j].ProcessName.value_;
                        info[i].ProcessStatusNo2 = -1;
                        continue;
                    }

                    if (2 == j)
                    {
                        info[i].ProcessIdNo3 = list[j].ProcessId.value_;
                        info[i].ProcessNameNo3 = list[j].ProcessName.value_;
                        info[i].ProcessStatusNo3 = -1;
                        continue;
                    }

                    if (3 == j)
                    {
                        info[i].ProcessIdNo4 = list[j].ProcessId.value_;
                        info[i].ProcessNameNo4 = list[j].ProcessName.value_;
                        info[i].ProcessStatusNo4 = -1;
                        continue;
                    }

                    if (4 == j)
                    {
                        info[i].ProcessIdNo5 = list[j].ProcessId.value_;
                        info[i].ProcessNameNo5 = list[j].ProcessName.value_;
                        info[i].ProcessStatusNo5 = -1;
                        continue;
                    }
                }
            }

            return true;
        }
        //更新进程状态
        public bool UpdateProcessStatus(string ep, List<Tool.Proto.ProcessStatus> list)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return false;
            }

            for (int i = 0; i < 5; i++)
            {
                if (list.Count < i + 1) break;
                
                if (varDict[ep].ProcessIdNo1 == list[i].ProcessId.value_)
                {
                    varDict[ep].ProcessStatusNo1 = list[i].ProcessCurStatus.value_;
                    continue;
                }

                if (varDict[ep].ProcessIdNo2 == list[i].ProcessId.value_)
                {
                    varDict[ep].ProcessStatusNo2 = list[i].ProcessCurStatus.value_;
                    continue;
                }

                if (varDict[ep].ProcessIdNo3 == list[i].ProcessId.value_)
                {
                    varDict[ep].ProcessStatusNo3 = list[i].ProcessCurStatus.value_;
                    continue;
                }

                if (varDict[ep].ProcessIdNo4 == list[i].ProcessId.value_)
                {
                    varDict[ep].ProcessStatusNo4 = list[i].ProcessCurStatus.value_;
                    continue;
                }

                if (varDict[ep].ProcessIdNo5 == list[i].ProcessId.value_)
                {
                    varDict[ep].ProcessStatusNo5 = list[i].ProcessCurStatus.value_;
                    continue;
                }
            }

            return true;
        }

        public int CheckProcessStatus(string ep, string name)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return -1;
            }

            if(varDict[ep].ProcessNameNo1 ==name)
            {
                return varDict[ep].ProcessStatusNo1;
            }

            if (varDict[ep].ProcessNameNo2 == name)
            {
                return varDict[ep].ProcessStatusNo2;
            }

            if (varDict[ep].ProcessNameNo3 == name)
            {
                return varDict[ep].ProcessStatusNo3;
            }

            if (varDict[ep].ProcessNameNo4 == name)
            {
                return varDict[ep].ProcessStatusNo4;
            }

            if (varDict[ep].ProcessNameNo5 == name)
            {
                return varDict[ep].ProcessStatusNo5;
            }

            return -1;
        }

        public bool UpdateSystemTime(string ep, Tool.Proto.ProtoSystemTime time)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return false;
            }

            varDict[ep].SystemTime = time.CurTime.value_;
            varDict[ep].VCUStatus = time.VCUStatus.value_;
            return true;
        }

        public bool UpdateFileLockStatus(string ep,int isLock)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return false;
            }
            varDict[ep].IsFileLock = isLock;
            return true;
        }

        public bool UpdateShellVersionInfo(string ep, string VersionInfo)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return false;
            }
            varDict[ep].VersionInfo = VersionInfo;
            return true;
        }

        public bool UpdateConfigVersionInfo(string ep, string info)
        {
            Dictionary<string, RobotInfo> varDict = Collection.ToList().ToDictionary(key => key.EndPoint, value => value);

            if (!varDict.ContainsKey(ep))
            {
                return false;
            }
            varDict[ep].ConfigInfo = info;
            return true;
        }

        public bool UpdateShellPercent(string ep, Int32 Finished)
        {
            int i = 0;
            foreach(var obj in Collection.ToList())
            {
                Collection.ToList()[i].ShellProcessPerc = Finished;
                i++;
            }
            return true;
        }



        ////////////////////////////以下函数记录已初始化的主机//////////////////////////////////
        public void AppendInitFileList(string ep)
        {
            if (InitFileList.Exists(o => o == ep))
            {
                return;
            }
            InitFileList.Add(ep);
        }

        public bool IsExitsInitFile(string ep)
        {
            if(!InitFileList.Exists(o=>o==ep))
            {
                return false;
            }
            return true;
        }

    }
}
