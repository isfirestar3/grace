using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Xml;

namespace install
{
    class RWXmlFile
    {
        public string GetXmlElement(string FilePath,string RootElement,string ChildElement)
        {
            //将XML文件加载进来
            XDocument document = XDocument.Load(FilePath);
            //获取到XML的根元素进行操作
            XElement root = document.Root;
            XElement ele = root.Element(RootElement);
            //获取name标签的值
            XElement child_element = ele.Element(ChildElement);
            if (child_element.IsEmpty) return "";
            else return child_element.Value;
        }

        public bool WriteXmlElement(string FilePath,string ParentElement,string ChildElement,string value)
        {
            try
            {
                //将XML文件加载进来
                XmlDocument doc = new XmlDocument();
                doc.Load(FilePath);    //加载Xml文件 
                XmlElement root = doc.DocumentElement;

                XmlNodeList personNode = root.GetElementsByTagName(ParentElement);
                if (personNode.Count == 0) return false;
                foreach (XmlNode node in personNode[0].ChildNodes)
                {
                    XmlElement ele = (XmlElement)node;
                    if (ele.Name.Equals(ChildElement))
                    {
                        ele.InnerText = value;
                    }
                }
                doc.Save(FilePath);
            }
            catch(Exception e)
            {
                Console.Write("RWXmlFile get an Exception: {0}", e.Message);
                return false;
            }
            return true;
        }

    }
}
