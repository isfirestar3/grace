using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;

namespace Tool.TransferPanel
{
    class TransColorConverter:IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            Int32 status = (Int32)value;

            SolidColorBrush brush = null;
            switch (status)
            {
                case -4:
                    brush = new SolidColorBrush(Colors.Red);
                    break;
                case -1:
                    brush = new SolidColorBrush(Colors.Red);
                    break;
                case -2:
                    brush = new SolidColorBrush(Colors.Red);
                    break;
                case 0:
                    brush = new SolidColorBrush(Colors.Green);
                    break;
                case 1:
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
