using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoMSG : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t MSG = new Proto.ProtoStringGB2312_t();        //
        
        public int length()
        {
            return MSG.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return MSG.serialize(byteStream, ref offset);

        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return MSG.build(byteStream, ref offset);

        }
    }
}
