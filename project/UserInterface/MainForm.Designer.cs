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
			this.MenuStrip = new System.Windows.Forms.MenuStrip ();
			this.FileMenu = new System.Windows.Forms.ToolStripMenuItem ();
			this.FileMenuOpen = new System.Windows.Forms.ToolStripMenuItem ();
			this.MenuStrip.SuspendLayout ();
			this.SuspendLayout ();
			// 
			// MenuStrip
			// 
			this.MenuStrip.Items.AddRange (new System.Windows.Forms.ToolStripItem[] {
            this.FileMenu});
			this.MenuStrip.Location = new System.Drawing.Point (0, 0);
			this.MenuStrip.Name = "MenuStrip";
			this.MenuStrip.Size = new System.Drawing.Size (284, 24);
			this.MenuStrip.TabIndex = 0;
			this.MenuStrip.Text = "MenuStrip";
			// 
			// FileMenu
			// 
			this.FileMenu.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem[] {
            this.FileMenuOpen});
			this.FileMenu.Name = "FileMenu";
			this.FileMenu.Size = new System.Drawing.Size (37, 20);
			this.FileMenu.Text = "File";
			// 
			// FileMenuOpen
			// 
			this.FileMenuOpen.Name = "FileMenuOpen";
			this.FileMenuOpen.Size = new System.Drawing.Size (152, 22);
			this.FileMenuOpen.Text = "Open";
			this.FileMenuOpen.Click += new System.EventHandler (this.FileMenuOpen_Click);
			// 
			// MainForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size (284, 262);
			this.Controls.Add (this.MenuStrip);
			this.MainMenuStrip = this.MenuStrip;
			this.Name = "MainForm";
			this.Text = "RayTracer";
			this.MenuStrip.ResumeLayout (false);
			this.MenuStrip.PerformLayout ();
			this.ResumeLayout (false);
			this.PerformLayout ();

		}

		#endregion

		private System.Windows.Forms.MenuStrip MenuStrip;
		private System.Windows.Forms.ToolStripMenuItem FileMenu;
		private System.Windows.Forms.ToolStripMenuItem FileMenuOpen;
	}
}

