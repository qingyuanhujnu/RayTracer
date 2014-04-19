using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.IO;
using Microsoft.Win32;
using System.Collections;

namespace UserInterface {
	public partial class MainForm : Form
    {
        private History history = new History ();
        private String openedFile = null;
        private int menuItemCount = 0;

		public MainForm ()
		{
			InitializeComponent ();
            
			configTextBox.SelectionTabs = new int[] { 15, 30, 45, 60, 75 };
            menuItemCount = fileMenu.DropDownItems.Count;

            history.Read ();
            history.AddAllMenuItems (fileMenu.DropDownItems, menuItemCount, this.HistoryClick);
            UpdateControlsForEdit ();
		}

        public void UpdateControlsForRender ()
        {
            menuStrip.Enabled = false;
            configTextBox.Enabled = false;
            pictureBox.Visible = false;
            progressBar.Visible = true;
            progressBar.Value = 0;
        }

        public void UpdateControlsForEdit ()
        {
            menuStrip.Enabled = true;
            configTextBox.Enabled = true;
            pictureBox.Visible = true;
            progressBar.Visible = false;
        }

        public void SetProgressBarValue (Int32 val)
        {
            progressBar.Value = val;
        }

        public void SetPictureBoxImage (Image image)
        {
            pictureBox.Image = image;
        }

		private void FileMenuOpen_Click (object sender, EventArgs e)
		{
			OpenFileDialog dialog = new OpenFileDialog ();
			if (dialog.ShowDialog () == DialogResult.OK) {
				OpenFile (dialog.FileName);
                history.AddMenuItem (fileMenu.DropDownItems, menuItemCount, dialog.FileName, this.HistoryClick);
                history.AddItem (dialog.FileName);
            }
		}

        private void FileMenuSave_Click (object sender, EventArgs e)
        {
            if (openedFile != null) {
                try {
                    StreamWriter writer = new StreamWriter (openedFile);
                    writer.Write (configTextBox.Text);
                    writer.Close ();
                } catch {
                }
            }
        }
        
        private void FileMenuExit_Click (object sender, EventArgs e)
        {
            Close ();
        }

        private void rayTraceMenu_Click (object sender, EventArgs e)
        {
            RayTracer rayTracer = new RayTracer (this, RayTracer.RenderMode.RayTraceMode);
            rayTracer.Start (configTextBox.Text);
        }

        private void pathTraceMenu_Click (object sender, EventArgs e)
        {
            RayTracer rayTracer = new RayTracer (this, RayTracer.RenderMode.PathTraceMode);
            rayTracer.Start (configTextBox.Text);
        }
        
        private void OpenFile (String fileName)
		{
			try {
				StreamReader reader = new StreamReader (fileName);
				configTextBox.Text = reader.ReadToEnd ();
				reader.Close ();
                openedFile = fileName;
			} catch {
			}
		}

        void HistoryClick (object sender, System.EventArgs e)
		{
			ToolStripMenuItem item = (ToolStripMenuItem)sender;
			OpenFile (item.Text);
		}

        private void MainForm_FormClosed (object sender, FormClosedEventArgs e)
        {
            history.Write ();
        }
    }
}
