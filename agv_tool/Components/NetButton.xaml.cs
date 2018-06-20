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
    public partial class NetButton : UserControl
    {

        //private int NetStatus_;

        public NetButton()
        {
            InitializeComponent();
        }

        //public int NetStatus
        //{
        //    get { return (int)GetValue(NetStatusProperty); }
        //    set { SetValue(NetStatusProperty, value); }
        //}

        //public static new readonly DependencyProperty NetStatusProperty = DependencyProperty.Register(
        //    "NetStatus",
        //    typeof(int),
        //    typeof(NetButton),
        //    new PropertyMetadata(null, new PropertyChangedCallback(OnNetStatusChanged)));


        //static void OnNetStatusChanged(object sender, DependencyPropertyChangedEventArgs args)
        //{
        //    NetButton Btn = sender as NetButton;
        //    Btn.NetStatus_ = (int)args.NewValue;
        //    Btn.UpdateGui(Btn.NetStatus_);
        //}

        //void UpdateGui(int Status)
        //{
        //    if (Status >= 0)
        //    {
        //        Tb.Text = "断开";
        //    }
        //    else
        //    {
        //        Tb.Text = "连接";
        //    }
        //}
    }
}
