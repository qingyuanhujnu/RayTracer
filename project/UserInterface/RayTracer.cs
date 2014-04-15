using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace UserInterface {
	static class Win32Functions
	{
		[DllImport ("kernel32.dll")]
		public static extern IntPtr LoadLibrary (string dllToLoad);
	}

	class RayTracer
	{
		public RayTracer ()
		{
		}

		public void Do ()
		{
			IntPtr rayTracerModule = Win32Functions.LoadLibrary ("RayTracer.dll"); 
		}
	}
}
