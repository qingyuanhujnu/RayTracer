using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.IO;

namespace UserInterface {
	static class Win32Functions
	{
		[DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
		[return: MarshalAs (UnmanagedType.I1)]
		public static extern bool RayTrace (
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
			//bool res = Win32Functions.RayTrace ("config01.txt", "result.png", 100, 100, 1.0);
		}
	}
}
