namespace UserInterface {
	partial class MainForm {
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose (bool disposing)
		{
			if (disposing && (components != null)) {
				components.Dispose ();
			}
			base.Dispose (disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent ()
		{
            this.MenuStrip = new System.Windows.Forms.MenuStrip();
            this.FileMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.FileMenuOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.FileMenuExit = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.RenderMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.SplitContainer = new System.Windows.Forms.SplitContainer();
            this.ConfigTextBox = new System.Windows.Forms.RichTextBox();
            this.PictureBox = new System.Windows.Forms.PictureBox();
            this.FileMenuSave = new System.Windows.Forms.ToolStripMenuItem();
            this.MenuStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.SplitContainer)).BeginInit();
            this.SplitContainer.Panel1.SuspendLayout();
            this.SplitContainer.Panel2.SuspendLayout();
            this.SplitContainer.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // MenuStrip
            // 
            this.MenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.FileMenu,
            this.RenderMenu});
            this.MenuStrip.Location = new System.Drawing.Point(0, 0);
            this.MenuStrip.Name = "MenuStrip";
            this.MenuStrip.Size = new System.Drawing.Size(584, 24);
            this.MenuStrip.TabIndex = 0;
            this.MenuStrip.Text = "MenuStrip";
            // 
            // FileMenu
            // 
            this.FileMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.FileMenuOpen,
            this.FileMenuSave,
            this.FileMenuExit,
            this.toolStripSeparator1});
            this.FileMenu.Name = "FileMenu";
            this.FileMenu.Size = new System.Drawing.Size(37, 20);
            this.FileMenu.Text = "File";
            // 
            // FileMenuOpen
            // 
            this.FileMenuOpen.Name = "FileMenuOpen";
            this.FileMenuOpen.Size = new System.Drawing.Size(152, 22);
            this.FileMenuOpen.Text = "Open";
            this.FileMenuOpen.Click += new System.EventHandler(this.FileMenuOpen_Click);
            // 
            // FileMenuExit
            // 
            this.FileMenuExit.Name = "FileMenuExit";
            this.FileMenuExit.Size = new System.Drawing.Size(152, 22);
            this.FileMenuExit.Text = "Exit";
            this.FileMenuExit.Click += new System.EventHandler(this.FileMenuExit_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(149, 6);
            // 
            // RenderMenu
            // 
            this.RenderMenu.Name = "RenderMenu";
            this.RenderMenu.Size = new System.Drawing.Size(56, 20);
            this.RenderMenu.Text = "Render";
            this.RenderMenu.Click += new System.EventHandler(this.RenderMenu_Click);
            // 
            // SplitContainer
            // 
            this.SplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SplitContainer.Location = new System.Drawing.Point(0, 24);
            this.SplitContainer.Name = "SplitContainer";
            // 
            // SplitContainer.Panel1
            // 
            this.SplitContainer.Panel1.Controls.Add(this.ConfigTextBox);
            // 
            // SplitContainer.Panel2
            // 
            this.SplitContainer.Panel2.Controls.Add(this.PictureBox);
            this.SplitContainer.Size = new System.Drawing.Size(584, 338);
            this.SplitContainer.SplitterDistance = 193;
            this.SplitContainer.TabIndex = 1;
            // 
            // ConfigTextBox
            // 
            this.ConfigTextBox.AcceptsTab = true;
            this.ConfigTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ConfigTextBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.ConfigTextBox.Location = new System.Drawing.Point(0, 0);
            this.ConfigTextBox.Name = "ConfigTextBox";
            this.ConfigTextBox.Size = new System.Drawing.Size(193, 338);
            this.ConfigTextBox.TabIndex = 0;
            this.ConfigTextBox.Text = "";
            this.ConfigTextBox.WordWrap = false;
            // 
            // PictureBox
            // 
            this.PictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.PictureBox.Location = new System.Drawing.Point(0, 0);
            this.PictureBox.Name = "PictureBox";
            this.PictureBox.Size = new System.Drawing.Size(387, 338);
            this.PictureBox.TabIndex = 0;
            this.PictureBox.TabStop = false;
            // 
            // FileMenuSave
            // 
            this.FileMenuSave.Name = "FileMenuSave";
            this.FileMenuSave.Size = new System.Drawing.Size(152, 22);
            this.FileMenuSave.Text = "Save";
            this.FileMenuSave.Click += new System.EventHandler(this.FileMenuSave_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(584, 362);
            this.Controls.Add(this.SplitContainer);
            this.Controls.Add(this.MenuStrip);
            this.MainMenuStrip = this.MenuStrip;
            this.Name = "MainForm";
            this.Text = "RayTracer";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.MenuStrip.ResumeLayout(false);
            this.MenuStrip.PerformLayout();
            this.SplitContainer.Panel1.ResumeLayout(false);
            this.SplitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.SplitContainer)).EndInit();
            this.SplitContainer.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.MenuStrip MenuStrip;
		private System.Windows.Forms.ToolStripMenuItem FileMenu;
		private System.Windows.Forms.ToolStripMenuItem FileMenuOpen;
		private System.Windows.Forms.SplitContainer SplitContainer;
		private System.Windows.Forms.RichTextBox ConfigTextBox;
        private System.Windows.Forms.ToolStripMenuItem FileMenuExit;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem RenderMenu;
		private System.Windows.Forms.PictureBox PictureBox;
        private System.Windows.Forms.ToolStripMenuItem FileMenuSave;
	}
}

