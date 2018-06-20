using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Tool.Net;

namespace Tool.CommView
{

    public static class WaitBox
    {
        public static void WaitEvent(this FrameworkElement owner,Action WaitEventCallback, bool ReportsProgress =true,bool responseClose = false)
        {
            WaitWindow win = new WaitWindow(responseClose);
            win.WaitEvent(WaitEventCallback, ReportsProgress);
            Window pwin = Tool.MainWindow.This;
            win.Owner = pwin;
            win.WindowStartupLocation = WindowStartupLocation.CenterOwner;
            win.ShowDialog();
        }
    }

    public partial class WaitWindow : Window
    {
        private Action WaitEventCallback;

        bool ResponseClose = false;
        public delegate void ProgressCallback(Int32 Status ,Int32 step);

        public static ProgressCallback ChangeCallback = new ProgressCallback(
            (Int32 status,Int32 setp) =>
            {
                Worker.ReportProgress(setp);
                return;
            }
        );

        public void WaitEvent(Action waitEvent, bool ReportsProgress)
        {
            WaitEventCallback = waitEvent;
           
            Worker = new BackgroundWorker();
            //更新进度  
            Worker.WorkerReportsProgress = ReportsProgress;
            //加载后台任务  
            Worker.DoWork += new DoWorkEventHandler(WorkerDoWork);
            //进度报告方法  
            Worker.ProgressChanged += new ProgressChangedEventHandler(WorkerProgressChanged);
            //后台任务执行完成时调用的方法  
            Worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(WorkerRunWorkerCompleted);
            //任务启动  
            Worker.RunWorkerAsync();
        }

        public WaitWindow(bool responseClose)
        {
            InitializeComponent();
            ResponseClose = responseClose;
        }
        
        static BackgroundWorker Worker;

        //执行任务  
        void WorkerDoWork(object sender, DoWorkEventArgs e)
        {
            //开始播放等待动画  
            this.Dispatcher.Invoke(new Action(() =>
            {
                loading.Visibility = System.Windows.Visibility.Visible;
            }));
            WaitEventCallback();
        }

        //报告任务进度  
        void WorkerProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                this.lab_pro.Content = "已完成"+e.ProgressPercentage + "%";
            }));
        }

        //任务执行完成后更新状态  
        void WorkerRunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            loading.Visibility = System.Windows.Visibility.Collapsed;
            this.Dispatcher.Invoke(new Action(() =>
            {
                Close();
                this.lab_pro.Content = "完成";
            }));
        }

        private void OnCloseWid(object sender, EventArgs e)
        {
            if(ResponseClose)
            {
                NetInterface.CancelPullOpreate();
            }
        }
    }
}
