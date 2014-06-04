using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Win32;

namespace UserInterface
{
    public class Settings
    {
        private Int32 resolution;

        public Settings ()
        {
            resolution = 32;
        }

        public Int32 Resolution
        {
            get
            {
                return resolution;
            }
            set 
            {
                resolution = value;
            }
        }

        public void Read ()
        {
            try {
                RegistryKey softwareKey = Registry.CurrentUser.OpenSubKey ("Software", true);
                RegistryKey appKey = softwareKey.CreateSubKey ("RayTracer");

                resolution = (Int32) appKey.GetValue ("resolution");

            } catch {
            }
        }

        public void Write ()
        {
            try {
                RegistryKey softwareKey = Registry.CurrentUser.OpenSubKey ("Software", true);
                RegistryKey appKey = softwareKey.CreateSubKey ("RayTracer");
                appKey.SetValue ("resolution", resolution);
            } catch {
            }
        }
    }
}
