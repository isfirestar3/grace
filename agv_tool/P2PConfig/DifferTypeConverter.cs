using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;

namespace Tool.P2PConfig
{
    class DifferTypeConverter:IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DifferType Type = (DifferType)value;

            SolidColorBrush brush = null;
            switch (Type)
            {
                case DifferType.CRC32:
                    brush = new SolidColorBrush(Colors.Blue);
                    break;
                case DifferType.None:
                    brush = new SolidColorBrush(Colors.Red);
                    break;
                case DifferType.Same:
                    brush = new SolidColorBrush(Colors.Black);
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
