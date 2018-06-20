
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
    class ProcessStatusColorConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Int32 status = (Int32)value;

            SolidColorBrush brush = null;
            switch (status)
            {
                case -1:
                    brush = new SolidColorBrush(Colors.Gray);
                    break;
                case 0:
                    brush = new SolidColorBrush(Colors.Red);
                    break;
                case 1:
                    brush = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#00c4c2")); ;
                    break;
            }
            return brush;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
