using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;

namespace Tool.TransferPanel
{
    public class TransStatusConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Int32 status = (Int32)value;

            string strStatus=null;
            switch(status)
            {
                case -4:
                    strStatus = "网络断开";
                    break;
                case -1:
                    strStatus = "写入失败";
                    break;
                case -2:
                    strStatus = "读取失败";
                    break;
                case 0:
                    strStatus = "正在传输";
                    break;
                case 1:
                    strStatus = "传输完成";
                    break;
                case 2:
                    strStatus = "等待传输";
                    break;
            }
            return strStatus;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }

    }
}
