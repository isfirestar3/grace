using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;
using Tool.RobotsHome;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;


namespace Tool.RobotsHome
{
    class LockFileStatusConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                if (values == null) return false;
                if (2 != values.Count()) return false;

                bool nst = (bool)values[0];
                if (!nst) return false;

                Int32 stst = (Int32)values[1];

                if (1 == stst) return false;
            }
            catch(Exception e)
            {
                Console.WriteLine("file lock status convert error:{0}", e.ToString());
                return false;
            }
            return true;
           
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            string[] splitValues = ((string)value).Split(' ');
            return splitValues;
        }
    }
}
