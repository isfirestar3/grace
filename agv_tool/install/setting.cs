using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace install
{
    
    public partial class setting : Form
    {
        private string targetDir;
        static AutoResetEvent mEvent = new AutoResetEvent(false);
        private string standardFolder;
        private string remoteFolder;

        public string TargetDir { get => targetDir; set => targetDir = value; }

        public setting(string target)
        {
            targetDir = target;
            InitializeComponent();
        }

        public void SettingInitlization()
        {
            RWXmlFile xml = new RWXmlFile();
            string xmlPath = targetDir + @"/AGVSetting.xml";
          
            string standardPath = xml.GetXmlElement(xmlPath, "agv_file_path", "local_standard_path");
            string remotePath = xml.GetXmlElement(xmlPath, "agv_file_path", "local_remote_path");
            if (standardPath.Equals("")) standardPath = targetDir + "standard";
            if (remotePath.Equals("")) remotePath = targetDir + "remote";
            standardBox.Text = standardPath.Replace("\\\\","/");
            remoteBox.Text = remotePath.Replace("\\\\", "/");
        }

        private void SavePathSetting(object sender, EventArgs e)
        {
            //去除双重斜杠\\ \\
            RWXmlFile xml = new RWXmlFile();
            string xmlPath = targetDir + @"AGVSetting.xml";
            if (!xml.WriteXmlElement(xmlPath, "agv_file_path", "local_standard_path", standardBox.Text.Replace('\\','/').Replace("//","/").Replace("\\\\","/")))
            {
                MessageBox.Show("failed to write AGVSetting.xml node.", "Warnning Message", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            if(!xml.WriteXmlElement(xmlPath, "agv_file_path", "local_remote_path", remoteBox.Text.Replace('\\', '/').Replace("//", "/").Replace("\\\\", "/")))
            {
                MessageBox.Show("failed to write AGVSetting.xml node.", "Warnning Message", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            Application.Exit();
        }

        private void StandardPath(object sender, EventArgs e)
        {
            try
            {
                var t = new Thread(() => {
                    if (folderDialog.ShowDialog() == DialogResult.OK)
                    {
                        standardFolder = folderDialog.SelectedPath;
                    }
                    else
                    {
                        standardFolder = "";
                    }
                    //触发信号
                    mEvent.Set();
                } );
                t.IsBackground = true;
                t.SetApartmentState(ApartmentState.STA);
                t.Start();
                //信号等待
                mEvent.WaitOne();
                if (!standardFolder.Equals(""))
                {
                    standardBox.Text = (standardFolder + @"\\standard").Replace("\\\\", "/").Replace('\\', '/');
                }
            }
            catch(Exception err)
            {
                MessageBox.Show(err.Message, "Error Message", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void AgvRemotePath(object sender, EventArgs e)
        {
            var t = new Thread(() => {
                if (folderDialog.ShowDialog() == DialogResult.OK)
                {
                    remoteFolder = (folderDialog.SelectedPath + @"\\remote").Replace("\\\\", "/").Replace('\\', '/');
                }
                else
                {
                    remoteFolder = "";
                }
                //触发信号
                mEvent.Set();
            });
            t.IsBackground = true;
            t.SetApartmentState(ApartmentState.STA);
            t.Start();
            //信号等待
            mEvent.WaitOne();
            if (!remoteFolder.Equals(""))
            {
                remoteBox.Text = remoteFolder;
            }
        }
    }

}
