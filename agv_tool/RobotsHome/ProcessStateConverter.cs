using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;


namespace Tool.RobotsHome
{
    class ProcessStateConverter :IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            bool status = (bool)values[1];

            if (!status) return false;

            return !((bool)values[0]);
        }
 
        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        { 
             return null;
        }

    }
}
