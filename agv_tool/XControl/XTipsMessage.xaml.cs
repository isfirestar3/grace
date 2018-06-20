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

namespace Tool.XControl
{
    /// <summary>
    /// Interaction logic for XTipsMessage.xaml
    /// </summary>
    public partial class XTipsMessage : Window
    {
        int ShowTime_;

        private XTipsMessage(int _ShowTime)
        {
            ShowTime_ = _ShowTime;
            InitializeComponent();
        }

        async void TipsMessage_Loaded(object sender, RoutedEventArgs e)
        {
            Title = showTB.Text;
            await Task.Run(() => {
                double decrease = 1 / ((double)ShowTime_ * 5);
                System.Threading.Thread.Sleep(ShowTime_ * 500);
                for (int i = 0; i < ShowTime_ * 5; ++i)
                {
                    System.Threading.Thread.Sleep(100);
                    try
                    {
                        Dispatcher.Invoke(() => {
                            showTB.Opacity -= decrease;
                        });
                    }
                    catch
                    {
                    }
                }
            });
            Close();
        }

        void OnComplate(IAsyncResult ar)
        {
            Dispatcher.Invoke(new Action(() => {
                Close();
            }));
        }

        public static void Show(FrameworkElement _Owner, Color _ShowColor, string _ShowText = null, int _ShowSize = 18, int _ShowTime = 3)
        {
            XTipsMessage win = new XTipsMessage(_ShowTime);
            Window pwin = Window.GetWindow(_Owner);
            win.Owner = pwin;
            pwin.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            win.showTB.Foreground = new SolidColorBrush(_ShowColor);
            win.showTB.FontSize = (double)_ShowSize;
            win.showTB.Text = _ShowText;
            var loc = _Owner.PointToScreen(new Point());
            win.Left = loc.X + (_Owner.ActualWidth - win.Width) / 2;
            win.Top = loc.Y + (_Owner.ActualHeight - win.Height) / 2;
            win.Show();
        }
    }

    public static class TipsMessageEx
    {
        public static void ShowTips(this FrameworkElement _Owner, string _ShowText = null, int _ShowSize = 18, int _ShowTime = 3, Color _ShowColor = new Color())
        {
            if (_ShowColor.Equals(new Color { }))
            {
                _ShowColor.A = 0xFF;
            }
            XTipsMessage.Show(_Owner, _ShowColor, _ShowText, _ShowSize, _ShowTime);
        }
    }
}
