using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;

namespace Tool.UpdateBin
{
    class CoreStatusConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Int32 status = (Int32)value;

            string text = null;

            switch (status)
            {
                case 0:
                    text = "完成";
                    break;
                case 1:
                    text = "操作中";
                    break;
                case 2:
                    text = "正在重启";
                    break;
                case 3:
                    text = "正在下发文件";
                    break;
                case 4:
                    text = "下发文件成功";
                    break;
                case -100:
                    text = "读取VCU信息失败";
                    break;
                case -99:
                    text = "重启失败";
                    break;
                case -98:
                    text = "升级失败";
                    break;
                case -97:
                    text = "下载文件失败";
                    break;
                 case -96:
                    text = "文件比对失败";
                    break;
                case -95:
                    text = "请求超时";
                    break;
                case -94:
                    text = "启用can转发失败";
                    break;
                case -93:
                    text = "can协议初始化ROM失败";
                    break;
            }

            return text;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
