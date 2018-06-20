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

namespace Tool.CommView
{
    /// <summary>
    /// Interaction logic for MsgBox.xaml
    /// </summary>
    public partial class MsgBox : Window
    {
        private bool CloseFlag_;
        private MessageBoxButton ButtonType_ { get; set; }
        public MessageBoxResult Result_ { get; private set; }
        public MsgBox(FrameworkElement _Owner, MessageBoxButton _type)
        {
            InitializeComponent();
            ButtonType_ = _type;
            CloseFlag_ = false;
            Window pwin = Window.GetWindow(_Owner);
            this.Owner = pwin;
            WindowStartupLocation = WindowStartupLocation.CenterOwner;
        }

        private void First_Button_Click(object sender, RoutedEventArgs e)
        {
            switch (ButtonType_)
            {
                case MessageBoxButton.OK:
                    Result_ = MessageBoxResult.OK;
                    break;
                case MessageBoxButton.OKCancel:
                    Result_ = MessageBoxResult.OK;
                    break;
                case MessageBoxButton.YesNo:
                    Result_ = MessageBoxResult.Yes;
                    break;
                case MessageBoxButton.YesNoCancel:
                    Result_ = MessageBoxResult.Yes;
                    break;
            }

            CloseFlag_ = true;
            Close();
        }
        private void Second_Button_Click(object sender, RoutedEventArgs e)
        {
            switch (ButtonType_)
            {
                case MessageBoxButton.OK:
                    break;
                case MessageBoxButton.OKCancel:
                case MessageBoxButton.YesNo:
                    return;
                case MessageBoxButton.YesNoCancel:
                    Result_ = MessageBoxResult.No;
                    break;
            }

            CloseFlag_ = true;
            Close();
        }
        private void Third_Button_Click(object sender, RoutedEventArgs e)
        {
            switch (ButtonType_)
            {
                case MessageBoxButton.OK:
                    return;
                case MessageBoxButton.OKCancel:
                    Result_ = MessageBoxResult.Cancel;
                    break;
                case MessageBoxButton.YesNo:
                    Result_ = MessageBoxResult.No;
                    break;
                case MessageBoxButton.YesNoCancel:
                    Result_ = MessageBoxResult.Cancel;
                    break;
            }

            CloseFlag_ = true;
            Close();
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (!CloseFlag_)
            {
                e.Cancel = true;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Title = TitleTB.Text;
            switch (ButtonType_)
            {
                case MessageBoxButton.OK:
                    SecondBT.Visibility = Visibility.Collapsed;
                    ThirdBT.Visibility = Visibility.Collapsed;
                    FirstBT.Text = "确  定";
                    break;
                case MessageBoxButton.OKCancel:
                    SecondBT.Visibility = Visibility.Collapsed;
                    FirstBT.Text = "确  定";
                    ThirdBT.Text = "取  消";
                    break;
                case MessageBoxButton.YesNo:
                    SecondBT.Visibility = Visibility.Collapsed;
                    FirstBT.Text = "是";
                    ThirdBT.Text = "否";
                    break;
                case MessageBoxButton.YesNoCancel:
                    FirstBT.Text = "是";
                    SecondBT.Background = (SolidColorBrush)FindResource("secondaryButton");
                    SecondBT.Text = "否";
                    ThirdBT.Text = "忽略";
                    break;
            }
        }

        private void Window_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                DragMove();
            }
        }

        public static MessageBoxResult Notify(FrameworkElement _Owner, string _Msg, string _Title = null, MessageBoxButton _Type = MessageBoxButton.OK, double _FontSize = 14)
        {
            MsgBox nfm = new MsgBox(_Owner, _Type);
            nfm.TitleTB.Text = _Title;
            nfm.MessageTB.Text = _Msg;
            nfm.FontSize = _FontSize;
            nfm.ShowDialog();

            return nfm.Result_;
        }
    }

    public static class NotifyMessageEx
    {
        public static MessageBoxResult ShowNotify(this FrameworkElement _Owner, string _Msg, string _Title = null, MessageBoxButton _Type = MessageBoxButton.OK, double _FontSize = 14)
        {
            return MsgBox.Notify(_Owner, _Msg, _Title, _Type, _FontSize);
        }
    }
}
