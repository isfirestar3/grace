using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Tool.Proto;
namespace Tool.Proto
{
    public class ProtoProcessTable : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<ProcessTable> TableList = new Proto.ProtoList_t<ProcessTable>(); 
        public List<ProcessTable> ProcessTableList
        {
            get
            {
                return TableList;
            }
            set
            {
                TableList.Clear();
                TableList.AddRange(value);
            }
        }
           
        public int length()
        {
            return TableList.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return TableList.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return TableList.build(byteStream, ref _offset);
        }
    };

    public class ProcessTable:ProtoInterface_t
    {
        public Tool.Proto.ProtoCRT_t<Int32> ProcessId = new Proto.ProtoCRT_t<Int32>();        // 索引
        public Tool.Proto.ProtoStringGB2312_t ProcessName = new Proto.ProtoStringGB2312_t();    // 进程名


        public int length()
        {
            return ProcessId.length()+ ProcessName.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return ProcessId.serialize(byteStream,ref offset) && ProcessName.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return ProcessId.build(byteStream, ref offset) && ProcessName.build(byteStream, ref offset);
        }
    }
}
