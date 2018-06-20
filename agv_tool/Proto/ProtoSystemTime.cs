using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{

    public class ProtoSystemTime : ProtoInterface_t
    { 
        public Tool.Proto.ProtoStringGB2312_t CurTime = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoCRT_t<Int32>   VCUStatus = new Proto.ProtoCRT_t<Int32>();          //VCU 使能
        public int length()
        {
            return CurTime.length() + VCUStatus.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return CurTime.serialize(byteStream, ref offset) && VCUStatus.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return CurTime.build(byteStream, ref offset) && VCUStatus.build(byteStream, ref offset);
        }
    }
}
