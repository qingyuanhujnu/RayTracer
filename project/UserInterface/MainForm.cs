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
	public partial class MainForm : Form {
		RayTracer rayTracer = new RayTracer ();
		const String historyFileName = "history.txt";

		public MainForm ()
		{
			InitializeComponent ();
			FileMenu.DropDownItems.Add (new ToolStripSeparator ());
			ConfigTextBox.SelectionTabs = new int[] { 15, 30, 45, 60, 75 };
			ReadHistory ();
		}

		private void FileMenuOpen_Click (object sender, EventArgs e)
		{
			OpenFileDialog dialog = new OpenFileDialog ();
			if (dialog.ShowDialog () == DialogResult.OK) {
				OpenFile (dialog.FileName);
				AddToHistory (dialog.FileName);
				AddHistoryMenuItem (dialog.FileName);
			}
		}

		private void OpenFile (String fileName)
		{
			try {
				StreamReader reader = new StreamReader (fileName);
				ConfigTextBox.Text = reader.ReadToEnd ();
				reader.Close ();
			} catch {
			}
		}

		private void ReadHistory ()
		{
			try {
				StreamReader reader = new StreamReader (historyFileName, true);
				List<String> history = new List<String> ();
				String line;
				while ((line = reader.ReadLine ()) != null) {
					history.Add (line);
				}
				reader.Close ();
				for (int i = 0; i < history.Count; i++) {
					if (i >= history.Count - 5) {
						AddHistoryMenuItem (history[i]);
					}
				}
			} catch {
			}
		}
		
		private void AddToHistory (String fileName)
		{
			try {
				StreamWriter writer = new StreamWriter (historyFileName, true);
				writer.WriteLine (fileName);
				writer.Close ();
			} catch {
			}
		}

		private void AddHistoryMenuItem (String fileName)
		{
			ToolStripMenuItem item = new ToolStripMenuItem ();
			item.Text = fileName;
			item.Click += new EventHandler (this.HistoryClick);

			FileMenu.DropDownItems.Insert (2, item);
		}

		void HistoryClick (object sender, System.EventArgs e)
		{
			ToolStripMenuItem item = (ToolStripMenuItem)sender;
			OpenFile (item.Text);
		}
	}
}
