using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;

namespace Tool.ProcessManager
{
    class ProcessStateConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            int status = (Int32)value;
            string text = null;
            switch (status)
            {
                case -1:
                    text = "未知";
                    break;
                case 0:
                    text = "停止";
                    break;
                case 1:
                    text = "运行";
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
