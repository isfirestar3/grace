using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


    namespace ProtoDefine
    {
    public class StringHelper
    {
        public static byte[] String2GB2312(string str)
        {
            if (null == str) return new byte[0];
            var gb2312 = System.Text.Encoding.GetEncoding("gb2312");
            byte[] r_byte = Encoding.Default.GetBytes(str);
            return Encoding.Convert(Encoding.Default, gb2312, r_byte);
        }
        public static string GB2312ToString(byte[] _data)
        {
            if (null == _data) return "";
            var gb2312 = System.Text.Encoding.GetEncoding("gb2312");
            byte[] r_byte = Encoding.Convert(gb2312, Encoding.Default, _data.TakeWhile(bt => 0 != bt).ToArray());
            return Encoding.Default.GetString(r_byte);
        }
    }
}
    


