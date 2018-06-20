using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration.Install;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace install
{
    [RunInstaller(true)]
    public partial class Installer : System.Configuration.Install.Installer
    {
        string LogicDir;
        public Installer()
        {
            InitializeComponent();
        }

        public override void Install(IDictionary stateSaver)
        {
            LogicDir = this.Context.Parameters["targetdir"];
            base.Install(stateSaver);
        }

        protected override void OnAfterInstall(IDictionary savedState)
        {
            setting config = new setting(LogicDir);
            config.SettingInitlization();
            config.ShowDialog();
            base.OnAfterInstall(savedState);
        }

    }
}
