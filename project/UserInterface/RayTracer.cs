using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.IO;
using System.Drawing;

namespace UserInterface {
	static class Win32Functions
	{
		[DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int RayTrace (
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
			int resolutionX,
			int resolutionY,
			double distance);
	}

	class RayTracer
	{
		public RayTracer ()
		{
		}

        public void Do (String configString, PictureBox pictureBox)
        {
            try {
                String tempFileName = "temp.txt";
                String tempResultFileName = "temp.png";

                StreamWriter writer = new StreamWriter (tempFileName);
                writer.Write (configString);
                writer.Close ();

                int res = Win32Functions.RayTrace (tempFileName, tempResultFileName, 200, 200, 1.0);
                if (res == 0) {
                    MemoryStream memoryStream = new MemoryStream (File.ReadAllBytes (tempResultFileName));
                    Image image = Image.FromStream (memoryStream);
                    pictureBox.Image = image;
                } else {
                    pictureBox.Image = null;
                }

                File.Delete (tempFileName);
                File.Delete (tempResultFileName);
            } catch {

            }
        }
	}
}
