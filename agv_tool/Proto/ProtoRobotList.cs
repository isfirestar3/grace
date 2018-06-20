using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace Tool.Proto
{
    public class ProtoRobotList : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<ProtoRobotInfo> List = new Proto.ProtoList_t<ProtoRobotInfo>();
        public List<ProtoRobotInfo> RobotList
        {
            get
            {
                return List;
            }
            set
            {
                List.Clear();
                List.AddRange(value);
            }
        }

        public int length()
        {
            return List.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return List.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return List.build(byteStream, ref _offset);
        }
    };

    public class ProtoRobotInfo : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t IPV4 = new Proto.ProtoStringGB2312_t();       
        public Tool.Proto.ProtoCRT_t<Int32> AGVPort = new Proto.ProtoCRT_t<Int32>();          
        public Tool.Proto.ProtoCRT_t<Int32> FTSPort = new Proto.ProtoCRT_t<Int32>();
        public Tool.Proto.ProtoStringGB2312_t Mac = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoCRT_t<Int32> CarID = new Proto.ProtoCRT_t<Int32>();

        public int length()
        {
            return IPV4.length() + AGVPort.length() + FTSPort.length() + Mac.length() + CarID.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return IPV4.serialize(byteStream, ref offset) && AGVPort.serialize(byteStream, ref offset) && FTSPort.serialize(byteStream, ref offset) && Mac.serialize(byteStream, ref offset) && CarID.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return IPV4.build(byteStream, ref offset) && AGVPort.build(byteStream, ref offset) && FTSPort.build(byteStream, ref offset) && Mac.build(byteStream, ref offset) && CarID.build(byteStream, ref offset);
        }
    }
}
