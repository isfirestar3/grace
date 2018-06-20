using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tool.Proto
{
    public class ProtoFolderList : ProtoInterface_t
    {
        Tool.Proto.ProtoList_t<FolderList> folderList = new Proto.ProtoList_t<FolderList>();
        public List<FolderList> FolderList
        {
            get
            {
                return folderList;
            }
            set
            {
                folderList.Clear();
                folderList.AddRange(value);
            }
        }

        public int length()
        {
            return folderList.length();
        }

        public bool serialize(byte[] byteStream, ref Int32 _offset)
        {
            return folderList.serialize(byteStream, ref _offset);
        }

        public bool build(byte[] byteStream, ref Int32 _offset)
        {
            return folderList.build(byteStream, ref _offset);
        }
    };

    public class FolderList : ProtoInterface_t
    {
        public Tool.Proto.ProtoStringGB2312_t FolderName = new Proto.ProtoStringGB2312_t();        //

        public int length()
        {
            return FolderName.length() ;
        }

        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            return FolderName.serialize(byteStream, ref offset);
                
        }

        public bool build(byte[] byteStream, ref Int32 offset)
        {
            return FolderName.build(byteStream, ref offset);
               
        }
    }
}
