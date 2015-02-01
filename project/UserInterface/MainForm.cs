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
using System.Drawing.Imaging;

namespace UserInterface {
	public partial class MainForm : Form
    {
        private History history = new History ();
        private Settings settings = new Settings ();
        private String openedFile = null;
        private int menuItemCount = 0;

		public MainForm ()
		{
			InitializeComponent ();
            
			configTextBox.SelectionTabs = new int[] { 15, 30, 45, 60, 75 };
            menuItemCount = fileMenu.DropDownItems.Count;

            history.Read ();
            history.AddAllMenuItems (fileMenu.DropDownItems, menuItemCount, this.HistoryClick);

            settings.Read ();
            UpdateControlsForEdit ();

			String lastHistoryItem = history.GetLastItem ();
			if (lastHistoryItem != null) {
				OpenFile (lastHistoryItem);
			}
		}

        public void UpdateControlsForRender ()
        {
            menuStrip.Enabled = false;
            configTextBox.Enabled = false;
            pictureBox.Visible = true;
            toolStripProgressBar.Visible = true;
            toolStripProgressBar.Value = 0;
            toolStripStatusLabel.Text = "";
        }

        public void UpdateControlsForEdit ()
        {
            menuStrip.Enabled = true;
            configTextBox.Enabled = true;
            pictureBox.Visible = true;
            toolStripProgressBar.Visible = false;
            toolStripStatusLabel.Text = "";
        }

        public void SetProgressBarValue (Int32 val)
        {
            toolStripProgressBar.Value = val;
            toolStripStatusLabel.Text = val.ToString () + "%";
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
            if (pictureBox.Image == null) {
                return;
            }

            SaveFileDialog dialog = new SaveFileDialog ();
            dialog.Filter = "PNG files (*.png)|*.png";
            if (dialog.ShowDialog () == DialogResult.OK) {
                pictureBox.Image.Save (dialog.FileName, ImageFormat.Png);
            }            
        }
        
        private void FileMenuExit_Click (object sender, EventArgs e)
        {
            Close ();
        }

        private void settingsMenu_Click (object sender, EventArgs e)
        {
            SettingsForm settingsForm = new SettingsForm (settings);
            settingsForm.ShowDialog ();
        }

        private void rayTraceMenu_Click (object sender, EventArgs e)
        {
            Renderer rayTracer = new Renderer (this, Renderer.RenderMode.RayTraceMode);
            rayTracer.Start (configTextBox.Text, settings);
        }

        private void pathTraceMenu_Click (object sender, EventArgs e)
        {
            Renderer rayTracer = new Renderer (this, Renderer.RenderMode.OpenCLTraceMode);
            rayTracer.Start (configTextBox.Text, settings);
        }

        private void pathTrace2Menu_Click (object sender, EventArgs e)
        {
            Renderer rayTracer = new Renderer (this, Renderer.RenderMode.PathTrace2Mode);
            rayTracer.Start (configTextBox.Text, settings);
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
            settings.Write ();
            history.Write ();
        }

        private void MainForm_DragDrop (object sender, DragEventArgs e)
        {
            string[] fileNames = (string[]) e.Data.GetData (DataFormats.FileDrop);
            if (fileNames.Length > 0) {
                OpenFile (fileNames[0]);
                history.AddMenuItem (fileMenu.DropDownItems, menuItemCount, fileNames[0], this.HistoryClick);
                history.AddItem (fileNames[0]);
            }
        }

        private void MainForm_DragEnter (object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent (DataFormats.FileDrop)) {
                e.Effect = DragDropEffects.Copy;
            }
        }
    }
}
