using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoBackup : ProtoInterface_t
    {
        public Tool.Proto.ProtoList_t<ProtoObj> ObjList = new Proto.ProtoList_t<ProtoObj>();
        public Tool.Proto.ProtoStringGB2312_t MSG = new Proto.ProtoStringGB2312_t();

        public List<ProtoObj>List
        {
            get
            {
                return ObjList;
            }
            set
            {
                ObjList.Clear();
                ObjList.AddRange(value);
            }
        }

        public int length()
        {
            return MSG.length()+ ObjList.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return ObjList.serialize(byteStream, ref offset) && MSG.serialize(byteStream, ref offset);

        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return ObjList.build(byteStream, ref offset) && MSG.build(byteStream, ref offset);
        }
    }

    public class ProtoObj : ProtoInterface_t
    {
        public Tool.Proto.ProtoCRT_t<Int32> ID = new Proto.ProtoCRT_t<Int32>();
        public Tool.Proto.ProtoCRT_t<Int32> CarID = new Proto.ProtoCRT_t<Int32>();
        public int length()
        {
            return ID.length()+ CarID.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return ID.serialize(byteStream, ref offset) && CarID.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return ID.build(byteStream, ref offset)&& CarID.build(byteStream, ref offset);
        }
    }
}
