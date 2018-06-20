using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Tool.RobotsHome
{
    /// <summary>
    /// Interaction logic for WinPackageInfo.xaml
    /// </summary>
    public partial class WinPackageInfo : Window
    {
        public WinPackageInfo()
        {
            InitializeComponent();
        }

        private void OnClickCloseWid(object sender, RoutedEventArgs e)
        {
            e.Handled = true;

            Task.Run(() =>
            {
                this.Dispatcher.Invoke(() => { Close(); });
            });
        }
    }
}
