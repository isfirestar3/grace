using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoDeleteFiles : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<DeleteFiles> fileList = new Proto.ProtoList_t<DeleteFiles>();
        public List<DeleteFiles> FileListList
        {
            get
            {
                return fileList;
            }
            set
            {
                fileList.Clear();
                fileList.AddRange(value);
            }
        }

        public int length()
        {
            return fileList.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return fileList.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return fileList.build(byteStream, ref _offset);
        }
    };

    public class DeleteFiles : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t FilePath = new Proto.ProtoStringGB2312_t();
        public Tool.Proto.ProtoCRT_t<Int32> FileType = new Proto.ProtoCRT_t<Int32>();

        public int length()
        {
            return FilePath.length() + FileType.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return FilePath.serialize(byteStream, ref offset) && FileType.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return FilePath.build(byteStream, ref offset) && FileType.build(byteStream, ref offset);
        }
    }
}
