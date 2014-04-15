using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace UserInterface {
	static class Win32Functions
	{
		[DllImport ("RayTracer.dll")]
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
		}

		public void Do ()
		{
			bool res = Win32Functions.RayTrace ("config01.txt", "result.png", 100, 100, 1.0);
		}
	}
}
