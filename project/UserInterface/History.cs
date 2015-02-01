using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using Microsoft.Win32;

namespace UserInterface
{
    class History
    {
        const String historyKeyName = "history";
        const Char separator = '|';
        List<String> history = new List<String> ();

        public History ()
        {

        }

        public void AddItem (String fileName)
        {
            history.Add (fileName);
        }

        public void AddMenuItem (ToolStripItemCollection collection, int index, String fileName, EventHandler handler)
        {
            ToolStripMenuItem item = new ToolStripMenuItem ();
            item.Text = fileName;
            item.Click += new EventHandler (handler);
            collection.Insert (index, item);
        }

		public String GetLastItem ()
		{
			if (history.Count == 0) {
				return null;
			}
			return history[history.Count - 1]; 
		}

        public void AddAllMenuItems (ToolStripItemCollection collection, int index, EventHandler handler)
        {
            for (int i = 0; i < history.Count; i++) {
                AddMenuItem (collection, index, history[i], handler);
            }
        }
        
        public void Read ()
        {
            try {
                RegistryKey softwareKey = Registry.CurrentUser.OpenSubKey ("Software", true);
                RegistryKey appKey = softwareKey.CreateSubKey ("RayTracer");
                String historyString = (String) appKey.GetValue (historyKeyName);
                if (historyString != null) {
                    String[] splitted = historyString.Split (separator);
                    for (int i = 0; i < splitted.Length; i++) {
                        if (splitted[i].Length > 0) {
                            history.Add (splitted[i]);
                        }
                    }
                }
            } catch {
            }
        }

        public void Write ()
        {
            try {
                RegistryKey softwareKey = Registry.CurrentUser.OpenSubKey ("Software", true);
                RegistryKey appKey = softwareKey.CreateSubKey ("RayTracer");
                String historyString = "";
                for (int i = 0; i < history.Count; i++) {
                    if (history.Count - i <= 10) {
                        historyString += history[i] + separator;
                    }
                }
                appKey.SetValue (historyKeyName, historyString);
            } catch {
            }
        }
    }
}
