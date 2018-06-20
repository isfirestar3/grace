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
    /// Interaction logic for ImageTextButton.xaml
    /// </summary>
    public partial class ImageTextButton : UserControl
    {
        Brush GetfocusBorderBrush_;
        Brush lostfocus_borderbrush_;

        RoutedEventHandler click_event_handler_ = null;

        public ImageTextButton()
        {
            InitializeComponent();
            GetfocusBorderBrush_ = lostfocus_borderbrush_ = new SolidColorBrush(Colors.LightGray);
        }

        public RoutedEventHandler Click
        {
            get { return click_event_handler_; }
            set { click_event_handler_ = value; }
        }
        public Brush GetFocusBorderBrush
        {
            get { return GetfocusBorderBrush_; }
            set { GetfocusBorderBrush_ = value; }
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
        public ImageSource Image
        {
            get { return img.Source; }
            set { img.Source = value; }
        }

        public double ImageHeight
        {
            get { return img.Height; }
            set { img.Height = value; }
        }
        public double ImageWidth
        {
            get { return img.Width; }
            set { img.Width = value; }
        }
        public Stretch ImageStretch
        {
            get { return img.Stretch; }
            set { img.Stretch = value; }
        }
        public Thickness ImageMargin
        {
            get { return img.Margin; }
            set { img.Margin = value; }
        }
        public HorizontalAlignment ImageHorizontalAlignment
        {
            get { return img.HorizontalAlignment; }
            set { img.HorizontalAlignment = value; }
        }
        public VerticalAlignment ImageVerticalAlignment
        {
            get { return img.VerticalAlignment; }
            set { img.VerticalAlignment = value; }
        }
        public Thickness TextMargin
        {
            get { return tb.Margin; }
            set { tb.Margin = value; }
        }
        public HorizontalAlignment TextHorizontalAlignment
        {
            get { return tb.HorizontalAlignment; }
            set { tb.HorizontalAlignment = value; }
        }
        public VerticalAlignment TextVerticalAlignment
        {
            get { return tb.VerticalAlignment; }
            set { tb.VerticalAlignment = value; }
        }

        public new Brush Background
        {
            get { return (Brush)GetValue(BackgroundProperty); }
            set { SetValue(BackgroundProperty, value); }
        }

        public static new readonly DependencyProperty BackgroundProperty = DependencyProperty.Register(
            "Background",
            typeof(Brush),
            typeof(ImageTextButton),
            new PropertyMetadata(null, new PropertyChangedCallback(OnBackgroundChanged)));

        static void OnBackgroundChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            ImageTextButton source = (ImageTextButton)sender;
            source.bd.Background = (Brush)args.NewValue;
        }

        private void comm_get_focus(object sender, RoutedEventArgs e)
        {
            bd.BorderBrush = GetfocusBorderBrush_;
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
                bd.BorderBrush = GetfocusBorderBrush_;
            }
            else
            {
                bd.BorderBrush = lostfocus_borderbrush_;
            }
        }

        private void comm_click(object sender, RoutedEventArgs e)
        {
            click_event_handler_?.Invoke(this, e);
        }
    }
}
