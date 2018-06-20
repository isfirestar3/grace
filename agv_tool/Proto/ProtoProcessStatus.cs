using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoProcessStatus : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<ProcessStatus> StatusList = new Proto.ProtoList_t<ProcessStatus>();
        public List<ProcessStatus> ProcessStatusList
        {
            get
            {
                return StatusList;
            }
            set
            {
                StatusList.Clear();
                StatusList.AddRange(value);
            }
        }

        public int length()
        {
            return StatusList.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return StatusList.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return StatusList.build(byteStream, ref _offset);
        }
    };

    public class ProcessStatus : ProtoInterface_t
    {
        public Tool.Proto.ProtoCRT_t<Int32> ProcessId = new Proto.ProtoCRT_t<Int32>();           // 索引
        public Tool.Proto.ProtoCRT_t<Int32> ProcessCurStatus = new Proto.ProtoCRT_t<Int32>();    // 进程状态
       

        public int length()
        {
            return ProcessId.length() + ProcessCurStatus.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return ProcessId.serialize(byteStream, ref offset) && ProcessCurStatus.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return ProcessId.build(byteStream, ref offset) && ProcessCurStatus.build(byteStream, ref offset);
        }
    }
}
