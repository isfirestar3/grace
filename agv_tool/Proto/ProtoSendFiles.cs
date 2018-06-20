using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class FileDir : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t FileName = new Proto.ProtoStringGB2312_t();        //

        public int length()
        {
            return FileName.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return FileName.serialize(byteStream, ref offset);

        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return FileName.build(byteStream, ref offset);
        }
    }

    public class ProtoSendFiles : ProtoInterface_t
    {
        public Tool.Proto.ProtoList_t<FileDir> FileList = new Proto.ProtoList_t<FileDir>();
        public Tool.Proto.ProtoStringGB2312_t ToDir    = new Proto.ProtoStringGB2312_t();

        public int length()
        {
            return FileList.length() + ToDir.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return FileList.serialize(byteStream, ref offset) && ToDir.serialize(byteStream, ref offset);
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return FileList.build(byteStream, ref offset) && ToDir.build(byteStream, ref offset);
        }
    }
}
