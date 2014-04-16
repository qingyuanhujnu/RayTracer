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
			ConfigTextBox.SelectionTabs = new int[] { 15, 30, 45, 60, 75 };
            menuItemCount = FileMenu.DropDownItems.Count;

            history.Read ();
            history.AddAllMenuItems (FileMenu.DropDownItems, menuItemCount, this.HistoryClick);
		}

		private void FileMenuOpen_Click (object sender, EventArgs e)
		{
			OpenFileDialog dialog = new OpenFileDialog ();
			if (dialog.ShowDialog () == DialogResult.OK) {
				OpenFile (dialog.FileName);
                history.AddMenuItem (FileMenu.DropDownItems, menuItemCount, dialog.FileName, this.HistoryClick);
                history.AddItem (dialog.FileName);
            }
		}

        private void FileMenuSave_Click (object sender, EventArgs e)
        {
            if (openedFile != null) {
                try {
                    StreamWriter writer = new StreamWriter (openedFile);
                    writer.Write (ConfigTextBox.Text);
                    writer.Close ();
                } catch {
                }
            }
        }
        
        private void FileMenuExit_Click (object sender, EventArgs e)
        {
            Close ();
        }

        private void RenderMenu_Click (object sender, EventArgs e)
        {
            RayTracer rayTracer = new RayTracer ();
            rayTracer.Do (ConfigTextBox.Text, PictureBox);
        }
        
        private void OpenFile (String fileName)
		{
			try {
				StreamReader reader = new StreamReader (fileName);
				ConfigTextBox.Text = reader.ReadToEnd ();
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
