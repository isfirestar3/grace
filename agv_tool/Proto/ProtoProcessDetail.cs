using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{

    public class ProtoProcessDetail : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<ProcessDetail> list = new Proto.ProtoList_t<ProcessDetail>();
        public List<ProcessDetail> List
        {
            get
            {
                return list;
            }
            set
            {
                list.Clear();
                list.AddRange(value);
            }
        }

        public int length()
        {
            return list.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return list.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return list.build(byteStream, ref _offset);
        }
    };

    public class ProcessDetail : ProtoInterface_t
    {
        public Tool.Proto.ProtoCRT_t<Int32>   ID         = new Proto.ProtoCRT_t<Int32>();
        public Tool.Proto.ProtoStringGB2312_t ProcessName = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoStringGB2312_t ProcessDir = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoStringGB2312_t ProcessArg = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoCRT_t<Int32>   ProcessDelayTime = new Proto.ProtoCRT_t<Int32>();

        public int length()
        {
            return ID.length()+ ProcessName.length()+ ProcessDir.length()+ ProcessArg.length()+ ProcessDelayTime.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return ID.serialize(byteStream, ref offset)&& ProcessName.serialize(byteStream, ref offset)&& ProcessDir.serialize(byteStream, ref offset) && ProcessArg.serialize(byteStream, ref offset) && ProcessDelayTime.serialize(byteStream, ref offset);

        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return ID.build(byteStream, ref offset) &&  ProcessName.build(byteStream, ref offset) && ProcessDir.build(byteStream, ref offset) && ProcessArg.build(byteStream, ref offset) && ProcessDelayTime.build(byteStream, ref offset);

        }
    }
}
