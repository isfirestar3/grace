using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;



namespace Tool.Proto
{
    public class ProtoMuteStatus : ProtoInterface_t
    {
        public Tool.Proto.ProtoCRT_t<Int32> IsLock = new Tool.Proto.ProtoCRT_t<Int32>();  

        public int length()
        {
            return IsLock.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return IsLock.serialize(byteStream, ref offset);

        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return IsLock.build(byteStream, ref offset);

        }
    }
}


