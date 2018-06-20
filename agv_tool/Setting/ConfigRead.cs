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
using Tool.RobotsHome;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.IO;

namespace Tool.Setting
{
     static public class ConfigRead
    {

        static string XMLLocalDir = null;
        static string PublishTime = null;
        static int    BlockSize   = 1;
        static string DHCP_IPV4 = "";
        static string DHCP_Port = "";
        static public void Read()
        {
            //读取版本信息
            try
            {
                XDocument XDoc = XDocument.Load("AGVSetting.xml");
                XElement rootNode = XDoc.Element("root");

                XElement nodeDir = rootNode.Element("agv_file_path");
                XMLLocalDir = nodeDir.Element("local_standard_path").Value.ToString();
                if (!Directory.Exists(XMLLocalDir))
                {
                    Directory.CreateDirectory(XMLLocalDir);
                }

                XElement nodeTime = rootNode.Element("PublishTime");
                PublishTime = nodeTime.FirstNode.ToString();

                XElement nodeBlockSize = rootNode.Element("BlockSize");
                BlockSize = Convert.ToInt32(nodeBlockSize.FirstNode.ToString());

                

                Tool.MainWindow.This.SetBlockIndex(BlockSize);
            }
            catch (Exception)
            {

            }
        }

        static public void ReadConfig(ref string localDir,  ref string publishTime)
        {
            localDir = XMLLocalDir;
            publishTime = PublishTime;
        }

        static public void UpdataLocalDir(string dir)
        {
            XMLLocalDir = dir;

            if (!Directory.Exists(XMLLocalDir))
            {
                Directory.CreateDirectory(XMLLocalDir);
            }

            try
            {
                XDocument XDoc = XDocument.Load("AGVSetting.xml");
                XElement rootNode = XDoc.Element("root");

                XElement nodeDir = rootNode.Element("agv_file_path");
                nodeDir.SetElementValue("local_standard_path", dir);
                XDoc.Save("AGVSetting.xml");
            }
            catch (Exception)
            {
                //此处需要添加错误提示
            }
        }

        static public void ReadLocalDir(ref string dir)
        {
            dir = XMLLocalDir;
        }

        static public void UpdataBlockSize(int index)
        {
            try
            {
                XDocument XDoc = XDocument.Load("AGVSetting.xml");
                XElement rootNode = XDoc.Element("root");
                XElement node = rootNode.Element("BlockSize");
                node.SetValue(Convert.ToString(index));
                XDoc.Save("AGVSetting.xml");
            }
            catch (Exception)
            {
                //此处需要添加错误提示
            }
        }

        static public void ReadDHCP(ref string ipv4, ref string port)
        {
            try
            {
                XDocument XDoc = XDocument.Load("AGVSetting.xml");
                XElement rootNode = XDoc.Element("root");
                XElement nodeDhcp = rootNode.Element("dhcp_addr");
                DHCP_IPV4 = nodeDhcp.Attribute("ipv4").Value.ToString();
                DHCP_Port = nodeDhcp.Attribute("port").Value.ToString();

                ipv4 = DHCP_IPV4;
                port = DHCP_Port;
            }
            catch
            {
                //此处需要添加错误提示
            }
        }

        static public int UpdataDHCP(string ipv4, string port)
        {
            try
            {
                XDocument XDoc = XDocument.Load("AGVSetting.xml");
                XElement rootNode = XDoc.Element("root");
                XElement node = rootNode.Element("dhcp_addr");
                node.SetAttributeValue("ipv4",ipv4);
                node.SetAttributeValue("port", port);
                XDoc.Save("AGVSetting.xml");
            }
            catch (Exception)
            {
                //此处需要添加错误提示
                return -1;
            }

            return 0;
        }

    }
}
