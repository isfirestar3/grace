using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
    

namespace Tool.Proto
{
        public class ProtoVCUEnable : ProtoInterface_t
        {
            public Tool.Proto.ProtoCRT_t<Int32> VCUEnable = new Proto.ProtoCRT_t<Int32>();         //开启关闭 0：关闭  1：开启
            public int length()
            {
                return VCUEnable.length();
            }

            public bool serialize(byte[] byteStream, ref Int32 offset)
            {
                return VCUEnable.serialize(byteStream, ref offset);
            }

            public bool build(byte[] byteStream, ref Int32 offset)
            {
                return VCUEnable.build(byteStream, ref offset);
            }
        }
}

