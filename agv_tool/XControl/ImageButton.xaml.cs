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

namespace Tool.XControl
{
    public class DetailReportEventArgs:RoutedEventArgs
    {
        public DetailReportEventArgs(RoutedEvent e, object source) : base(e, source) { }
        public string Location { set; get;}

    }

    public partial class ImageButton : UserControl
    {
        enum ButtonStatus
        {
            BS_ENTER,
            BS_LEAVE,
            BS_PRESS,
            BS_DISABLED
        }

        private ButtonStatus ButtonFlg_ = ButtonStatus.BS_LEAVE;
        private ImageSource EnterImage_;
        private ImageSource LeaveImage_;
        private ImageSource PressImage_;
        private ImageSource DisabledImage_;

        public static readonly RoutedEvent DetailReportEvent = EventManager.RegisterRoutedEvent("DetailReport",RoutingStrategy.Bubble,typeof(EventHandler<DetailReportEventArgs>),typeof(ImageButton));

        public event RoutedEventHandler DetailReport
        {
            add { this.AddHandler(DetailReportEvent, value); }
            remove { this.RemoveHandler(DetailReportEvent, value); }
        }


        public ImageButton()
        {
            InitializeComponent();
        }

        public ImageSource EnterImage
        {
            get { return (ImageSource)GetValue(EnterImageProperty); }
            set { SetValue(EnterImageProperty, value); }
        }

        public ImageSource LeaveImage
        {
            get { return (ImageSource)GetValue(LeaveImageProperty); }
            set {SetValue(LeaveImageProperty, value);}
        }

        public ImageSource PressImage
        {
            get { return (ImageSource)GetValue(PressImageProperty); }
            set { SetValue(PressImageProperty, value); }
        }

        public ImageSource DisabledImage
        {
            get { return (ImageSource)DisabledImage_; }
            set { DisabledImage_= value; }
        }

        public static readonly DependencyProperty EnterImageProperty = DependencyProperty.Register(
            "EnterImage",
            typeof(ImageSource),
            typeof(ImageButton),
            new PropertyMetadata(null, new PropertyChangedCallback(OnEnterImageChanged)));

        public static readonly DependencyProperty LeaveImageProperty = DependencyProperty.Register(
            "LeaveImage",
            typeof(ImageSource),
            typeof(ImageButton),
            new PropertyMetadata(null, new PropertyChangedCallback(OnLeaveImageChanged)));

        public static readonly DependencyProperty PressImageProperty = DependencyProperty.Register(
            "PressImage",
            typeof(ImageSource),
            typeof(ImageButton),
            new PropertyMetadata(null, new PropertyChangedCallback(OnPressImageChanged)));

        

        static void OnEnterImageChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            ImageButton source = (ImageButton)sender;
            source.EnterImage_ = (ImageSource)args.NewValue;
            source.adjust_view();
        }
        static void OnLeaveImageChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            ImageButton source = (ImageButton)sender;
            source.LeaveImage_ = (ImageSource)args.NewValue;
            source.adjust_view();
        }
        void OnDisabledImageChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            //Grid source = (Grid)sender;
            //if (!((bool)args.NewValue)){
            //   LeaveImage = DisabledImage_;
            //   adjust_view();
            //}
            //else
            //{
            //    LeaveImage = DisabledImage_;
            //    adjust_view();
            //}
            
        }

        static void OnPressImageChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            ImageButton source = (ImageButton)sender;
            source.PressImage_ = (ImageSource)args.NewValue;
            source.adjust_view();
        }

        private void OnMouseEnter(object sender, MouseEventArgs e)
        {
            ButtonFlg_ = ButtonStatus.BS_ENTER;
            adjust_view();
        }

        private void OnMouseLeave(object sender, MouseEventArgs e)
        {
            ButtonFlg_ = ButtonStatus.BS_LEAVE;
            adjust_view();
        }

        private void OnMousePress(object sender, MouseEventArgs e)
        {
            ButtonFlg_ = ButtonStatus.BS_PRESS;
            adjust_view();
        }

        private void adjust_view()
        {
            switch (ButtonFlg_)
            {
                case ButtonStatus.BS_LEAVE:
                    ImageView.Source = LeaveImage_;
                    break;
                case ButtonStatus.BS_ENTER:
                    ImageView.Source = EnterImage_;
                    break;
                case ButtonStatus.BS_PRESS:
                    ImageView.Source = PressImage_;
                    break;
                case ButtonStatus.BS_DISABLED:
                    ImageView.Source = DisabledImage_;
                    break;
            }
        }
    }
}
