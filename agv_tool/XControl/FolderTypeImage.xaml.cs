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
    /// <summary>
    /// Interaction logic for FolderTypeImage.xaml
    /// </summary>
    /// 

    public partial class FolderTypeImage : UserControl
    {
        public enum FileType
        {
            File,
            Folder,
            No,
        }

        public FolderTypeImage()
        {
            InitializeComponent();
        }

        private ImageSource imageshow;
        private FileType type = FileType.File;

        public ImageSource ImageShow
        {
            get { return (ImageSource)GetValue(ImageShowProperty); }
            set { SetValue(ImageShowProperty, value); }
        }

        public FileType Type
        {
            get { return type; }
            set { type = value; }
        }

        public static readonly DependencyProperty ImageShowProperty = DependencyProperty.Register(
            "ImageShow",
            typeof(ImageSource),
            typeof(FolderTypeImage),
            new PropertyMetadata(null, new PropertyChangedCallback(OnImageChanged)));

        static void OnImageChanged(object sender, DependencyPropertyChangedEventArgs args)
        {
            FolderTypeImage source = (FolderTypeImage)sender;
            source.imageshow = (ImageSource)args.NewValue;
            source.AdjustView();
        }

        private void AdjustView()
        {
            ImageView.Source = imageshow;
        }
    }
}
