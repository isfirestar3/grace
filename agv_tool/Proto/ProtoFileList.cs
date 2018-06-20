using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoFileList : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<ProtoFileInfo> fileList = new Proto.ProtoList_t<ProtoFileInfo>();
        public List<ProtoFileInfo> FileListList
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

    public class ProtoFileInfo : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t FileName       = new Proto.ProtoStringGB2312_t();        // 索引
        public Tool.Proto.ProtoCRT_t<Int32>   FileType       = new Proto.ProtoCRT_t<Int32>();          // 文件类型
        public Tool.Proto.ProtoCRT_t<Int32>   FileAttributes = new Proto.ProtoCRT_t<Int32>();          // 文件属性
        public Tool.Proto.ProtoCRT_t<UInt64>  FileSize       = new Proto.ProtoCRT_t<UInt64>();         // 文件大小
        public Tool.Proto.ProtoCRT_t<UInt64>  FileCRC32      = new Proto.ProtoCRT_t<UInt64>();         // CRC
        public Tool.Proto.ProtoCRT_t<UInt64>  FileCreateTime = new Proto.ProtoCRT_t<UInt64>();         // 创建时间
        public Tool.Proto.ProtoCRT_t<UInt64>  FileModifyTime = new Proto.ProtoCRT_t<UInt64>();         // 修改时间
        public Tool.Proto.ProtoCRT_t<UInt64>  FileAccessTime = new Proto.ProtoCRT_t<UInt64>();         // 修改时间
        public int length()
        {
            return FileName.length() + FileType.length() + FileAttributes.length() + FileSize.length() + FileCRC32.length() + FileCreateTime.length()+ FileModifyTime.length()+ FileAccessTime.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return FileName.serialize(byteStream, ref offset) && FileType.serialize(byteStream, ref offset) && FileAttributes.serialize(byteStream, ref offset) && FileSize.serialize(byteStream, ref offset)
                && FileCRC32.serialize(byteStream, ref offset) && FileCreateTime.serialize(byteStream, ref offset) && FileModifyTime.serialize(byteStream, ref offset)&& FileAccessTime.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return FileName.build(byteStream, ref offset) && FileType.build(byteStream, ref offset) && FileAttributes.build(byteStream, ref offset) && FileSize.build(byteStream, ref offset)
                && FileCRC32.build(byteStream, ref offset) && FileCreateTime.build(byteStream, ref offset) && FileModifyTime.build(byteStream, ref offset)&& FileAccessTime.build(byteStream, ref offset);
        }
    }
}
