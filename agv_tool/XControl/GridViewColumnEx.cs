using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;

namespace Tool.XControl
{
    public class GridViewColumnEx : GridViewColumn
    {

        bool ManualModify_;
        double WidthScale_;
        double GridViewLength_;

        public GridViewColumnEx(): base()
        {
            ManualModify_ = true;
            WidthScale_ = Double.NaN;
            GridViewLength_ = Double.NaN;
        }

        public double WidthScale
        {
            get
            {
                return (double)GetValue(WidthScaleProperty);
            }
            set
            {
                SetValue(WidthScaleProperty, value);
            }
        }
        public double GridViewLength
        {
            get
            {
                return (double)GetValue(GridViewLengthProperty);
            }
            set
            {
                SetValue(GridViewLengthProperty, value);
            }
        }

        public static readonly DependencyProperty WidthScaleProperty =
            DependencyProperty.Register("WidthScale", typeof(double),typeof(GridViewColumnEx),new PropertyMetadata(double.NaN, new PropertyChangedCallback(OnWidthScaleChanged)));

        public static readonly DependencyProperty GridViewLengthProperty =DependencyProperty.Register("GridViewLength", typeof(double),typeof(GridViewColumnEx),
            new PropertyMetadata(double.NaN, new PropertyChangedCallback(OnGridViewLengthChanged)));

        static void OnWidthScaleChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            GridViewColumnEx source = (GridViewColumnEx)sender;
            source.WidthScale_ = (double)args.NewValue;
            if (double.IsNaN(source.GridViewLength_) || double.IsNaN(source.WidthScale_))
            {
                return;
            }
            var s_base = source as GridViewColumn;
            source.ManualModify_ = false;
            s_base.Width = source.GridViewLength_ * source.WidthScale_;
            source.ManualModify_ = true;
        }
        static void OnGridViewLengthChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            GridViewColumnEx source = (GridViewColumnEx)sender;
            source.GridViewLength_ = (double)args.NewValue;
            if (double.IsNaN(source.GridViewLength_) || double.IsNaN(source.WidthScale_))
            {
                return;
            }
            var s_base = source as GridViewColumn;
            source.ManualModify_ = false;
            s_base.Width = source.GridViewLength_ * source.WidthScale_;
            source.ManualModify_ = true;
        }

        protected override void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            if (e.PropertyName == "Width" && ManualModify_)
            {
                if (double.IsNaN(GridViewLength_) || double.IsNaN(WidthScale_))
                {
                    return;
                }
                WidthScale_ = Width / GridViewLength_;
            }
        }
    }
}
