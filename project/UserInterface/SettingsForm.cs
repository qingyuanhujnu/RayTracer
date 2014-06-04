using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace UserInterface
{
    public partial class SettingsForm : Form
    {
        private Settings settings;

        public SettingsForm (Settings settings)
        {
            InitializeComponent ();

            this.settings = settings;
            resolutionNumber.Value = settings.Resolution;
        }
        
        private void okButton_Click (object sender, EventArgs e)
        {
            settings.Resolution = (Int32) resolutionNumber.Value;
            Close ();
        }
    }
}
