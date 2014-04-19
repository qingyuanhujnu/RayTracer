using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.ComponentModel;

namespace UserInterface {
	static class Win32Functions
	{
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void ProgressCallback (double progress);

		[DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int RayTrace (
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
            ProgressCallback progressCallback);
	}

	class RayTracer
    {
        class RayTracerWorker
        {
            private BackgroundWorker backgroundWorker = new BackgroundWorker ();

            private String tempFileName;
            private String tempResultFileName;

            private Action<int> progressCallback;
            private Action<int> finishedCallback;

            int result = -1;

            public RayTracerWorker ()
            {
                backgroundWorker.WorkerReportsProgress = true;
                backgroundWorker.DoWork += new DoWorkEventHandler (DoRayTrace);
                backgroundWorker.ProgressChanged += new ProgressChangedEventHandler (RayTraceProgressChanged);
                backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler (RayTraceCompleted);
            }

            public void Start (String tempFileName, String tempResultFileName, Action<int> progressCallback, Action<int> finishedCallback)
            {
                this.tempFileName = tempFileName;
                this.tempResultFileName = tempResultFileName;
                this.progressCallback = progressCallback;
                this.finishedCallback = finishedCallback;
                backgroundWorker.RunWorkerAsync ();
            }

            private void NativeRayTracerProgressCallback (double progress)
            {
                backgroundWorker.ReportProgress ((Int32) (progress * 100.0));
            }

            private void DoRayTrace (object sender, DoWorkEventArgs e)
            {
                BackgroundWorker worker = sender as BackgroundWorker;
                result = Win32Functions.RayTrace (tempFileName, tempResultFileName, NativeRayTracerProgressCallback);
            }

            private void RayTraceProgressChanged (object sender, ProgressChangedEventArgs e)
            {
                progressCallback (e.ProgressPercentage);
            }

            private void RayTraceCompleted (object sender, RunWorkerCompletedEventArgs e)
            {
                finishedCallback (result);
            }
        }

        private MainForm mainForm;
        private String tempFileName;
        private String tempResultFileName;

		public RayTracer (MainForm mainForm)
		{
            this.mainForm = mainForm;
		}

        public void Start (String configString)
        {
            try {
                mainForm.UpdateControlsForRender ();
                mainForm.SetPictureBoxImage (null);

                tempFileName = "temp.txt";
                tempResultFileName = "temp.png";

                StreamWriter writer = new StreamWriter (tempFileName);
                writer.Write (configString);
                writer.Close ();

                RayTracerWorker worker = new RayTracerWorker ();
                worker.Start (tempFileName, tempResultFileName, ProgressCallback, FinishedCallback);
            } catch {

            }
        }

        private void ProgressCallback (int progress)
        {
            mainForm.SetProgressBarValue (progress);
        }

        private void FinishedCallback (int result)
        {
            if (result == 0) {
                MemoryStream memoryStream = new MemoryStream (File.ReadAllBytes (tempResultFileName));
                Image image = Image.FromStream (memoryStream);
                mainForm.SetPictureBoxImage (image);
            } else {
                mainForm.SetPictureBoxImage (null);
            }

            File.Delete (tempFileName);
            File.Delete (tempResultFileName);

            mainForm.UpdateControlsForEdit ();
        }
	}
}
