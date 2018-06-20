namespace install
{
    partial class setting
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(setting));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.saveBtn = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.remoteBtn = new System.Windows.Forms.Button();
            this.remoteBox = new System.Windows.Forms.TextBox();
            this.standardBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.standardBtn = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.folderDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.groupBox1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.saveBtn);
            this.groupBox1.Controls.Add(this.tabControl1);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(435, 490);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "设置：";
            // 
            // saveBtn
            // 
            this.saveBtn.Location = new System.Drawing.Point(354, 459);
            this.saveBtn.Name = "saveBtn";
            this.saveBtn.Size = new System.Drawing.Size(75, 23);
            this.saveBtn.TabIndex = 1;
            this.saveBtn.Text = "保存";
            this.saveBtn.UseVisualStyleBackColor = true;
            this.saveBtn.Click += new System.EventHandler(this.SavePathSetting);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Location = new System.Drawing.Point(6, 20);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(423, 435);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.remoteBtn);
            this.tabPage1.Controls.Add(this.remoteBox);
            this.tabPage1.Controls.Add(this.standardBox);
            this.tabPage1.Controls.Add(this.label2);
            this.tabPage1.Controls.Add(this.standardBtn);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(415, 409);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "本地设置";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // remoteBtn
            // 
            this.remoteBtn.Location = new System.Drawing.Point(375, 86);
            this.remoteBtn.Name = "remoteBtn";
            this.remoteBtn.Size = new System.Drawing.Size(34, 23);
            this.remoteBtn.TabIndex = 5;
            this.remoteBtn.Text = "...";
            this.remoteBtn.UseVisualStyleBackColor = true;
            this.remoteBtn.Visible = false;
            this.remoteBtn.Click += new System.EventHandler(this.AgvRemotePath);
            // 
            // remoteBox
            // 
            this.remoteBox.Location = new System.Drawing.Point(88, 87);
            this.remoteBox.Name = "remoteBox";
            this.remoteBox.Size = new System.Drawing.Size(281, 21);
            this.remoteBox.TabIndex = 4;
            // 
            // standardBox
            // 
            this.standardBox.Location = new System.Drawing.Point(88, 29);
            this.standardBox.Name = "standardBox";
            this.standardBox.Size = new System.Drawing.Size(281, 21);
            this.standardBox.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 90);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(89, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "车载保存路径：";
            // 
            // standardBtn
            // 
            this.standardBtn.Location = new System.Drawing.Point(375, 29);
            this.standardBtn.Name = "standardBtn";
            this.standardBtn.Size = new System.Drawing.Size(34, 23);
            this.standardBtn.TabIndex = 2;
            this.standardBtn.Text = "...";
            this.standardBtn.UseVisualStyleBackColor = true;
            this.standardBtn.Visible = false;
            this.standardBtn.Click += new System.EventHandler(this.StandardPath);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(89, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "标准模板路径：";
            // 
            // folderDialog
            // 
            this.folderDialog.Description = "请选择文件夹";
            // 
            // setting
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(459, 514);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "setting";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "安装设置";
            this.TopMost = true;
            this.groupBox1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button saveBtn;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.Button remoteBtn;
        private System.Windows.Forms.TextBox remoteBox;
        private System.Windows.Forms.TextBox standardBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button standardBtn;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.FolderBrowserDialog folderDialog;
    }
}