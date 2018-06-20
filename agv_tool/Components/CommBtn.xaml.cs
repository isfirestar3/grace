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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Tool.CommView
{
    /// <summary>
    /// Interaction logic for CommBtn.xaml
    /// </summary>
    public partial class CommBtn : UserControl
    {
        Brush enter_foreground_;
        Brush leave_foreground_;

        Brush getfocus_borderbrush_;
        Brush lostfocus_borderbrush_;

        RoutedEventHandler click_event_handler_ = null;
        public CommBtn()
        {
            enter_foreground_ = leave_foreground_ = new SolidColorBrush(Colors.Black);
            getfocus_borderbrush_ = lostfocus_borderbrush_ = new SolidColorBrush(Colors.LightGray);
            InitializeComponent();
        }
        public RoutedEventHandler Click
        {
            get { return click_event_handler_; }
            set { click_event_handler_ = value; }
        }
        public Brush EnterForeground
        {
            get { return enter_foreground_; }
            set { enter_foreground_ = value; }
        }
        public Brush LeaveForeground
        {
            get { return leave_foreground_; }
            set { leave_foreground_ = value; }
        }
        public Brush GetFocusBorderBrush
        {
            get { return getfocus_borderbrush_; }
            set { getfocus_borderbrush_ = value; }
        }
        public Brush LostFocusBorderBrush
        {
            get { return lostfocus_borderbrush_; }
            set { lostfocus_borderbrush_ = value; }
        }
        public CornerRadius CornerRadius
        {
            get { return bd.CornerRadius; }
            set { bd.CornerRadius = value; }
        }
        public string Text
        {
            get { return tb.Text; }
            set { tb.Text = value; }
        }

        private void comm_mouse_enter(object sender, MouseEventArgs e)
        {
            tb.Foreground = enter_foreground_;
        }

        private void comm_mouse_leave(object sender, MouseEventArgs e)
        {
            tb.Foreground = leave_foreground_;
        }

        private void comm_get_focus(object sender, RoutedEventArgs e)
        {
            bd.BorderBrush = getfocus_borderbrush_;
            if (bt.IsFocused)
            {
                e.Handled = true;
                return;
            }
            bt.Focus();
        }

        private void comm_lost_focus(object sender, RoutedEventArgs e)
        {
            bd.BorderBrush = lostfocus_borderbrush_;
        }

        private void comm_left_button_down(object sender, MouseButtonEventArgs e)
        {
            click_event_handler_?.Invoke(this, e);
        }

        private void comm_loaded(object sender, RoutedEventArgs e)
        {
            if (this.IsFocused)
            {
                bd.BorderBrush = getfocus_borderbrush_;
            }
            else
            {
                bd.BorderBrush = lostfocus_borderbrush_;
            }
            tb.Foreground = leave_foreground_;
        }

        private void comm_click(object sender, RoutedEventArgs e)
        {
            click_event_handler_?.Invoke(this, e);
        }
    }
}
