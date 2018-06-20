using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Globalization;
using System.Windows.Media;
using Tool.RobotsHome;

namespace Tool.ProcessConfig
{
    class ProcessListConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            string name="";
            if (0 == values.Length) return name;
            string ep = (string )values[0];

            for (int i =1; i< values.Length; i++)
            {
                if (Tool.RobotsHome.RobotInfoCollect.GetInstance().CheckProcessStatus(ep, (string)values[i]) < 0) 
                {
                    continue;
                }
                name += "    " + values[i];
            }
            return name;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            string[] splitValues = ((string)value).Split(' ');
            return splitValues;
        }
    }
}
