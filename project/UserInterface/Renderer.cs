using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.ComponentModel;
using System.Collections;

namespace UserInterface {
	static class Win32Functions
	{
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void StartRenderCallback (int picWidth, int picHeight, int vertexCount, int triangleCount);

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void EndRenderCallback ();

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void ProgressCallback (double progress);

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void SetPixelCallback(int x, int y, double r, double g, double b);

		[DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int Render (
            int algorithm,
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
            int sampleNum,
            StartRenderCallback startRenderCallback,
            EndRenderCallback endRenderCallback,
            ProgressCallback progressCallback,
            SetPixelCallback setPixelCallback);
    }

	class Renderer
    {
        private MainForm mainForm;
        private RenderMode renderMode;
        private String tempFileName;

        private volatile Bitmap renderImage;
		private volatile ArrayList pixelTable;
		private volatile Int32 finishedPixels;

        public enum RenderMode
        {
            RayTraceMode,
            OpenCLTraceMode,
            PathTrace2Mode
        };

        public Renderer (MainForm mainForm, RenderMode renderMode)
		{
            this.mainForm = mainForm;
            this.renderMode = renderMode;
            this.renderImage = null;
		}

        public void Start (String configString, Settings settings)
        {
            try {
                mainForm.UpdateControlsForRender ();
                mainForm.SetPictureBoxImage (null);

                tempFileName = "temp.txt";

                StreamWriter writer = new StreamWriter (tempFileName);
                writer.Write (configString);
                writer.Close ();

                RenderWorker worker = new RenderWorker ();
                worker.Start (renderMode, settings, tempFileName, StartRender, EndRender, Progress, Finish, SetPixel);
            } catch {

            }
        }

        private void StartRender (int picWidth, int picHeight, int vertexCount, int triangleCount)
        {
            if (renderImage != null) {
                renderImage.Dispose ();
            }
            renderImage = new Bitmap (picWidth, picHeight);

			pixelTable = ArrayList.Synchronized (new ArrayList ());
			finishedPixels = 0;
		}

        private void EndRender ()
        {
        }

        private void Progress (int progress)
        {
            mainForm.SetProgressBarValue(progress);

            lock (this)
            {
				Int32 pixelTableCount = pixelTable.Count;
				for (Int32 i = finishedPixels; i < pixelTableCount; i++) {
					DictionaryEntry entry = (DictionaryEntry)pixelTable[i];
					renderImage.SetPixel (((Point) entry.Key).X, renderImage.Height - ((Point) entry.Key).Y - 1, (Color) entry.Value);
				}
				finishedPixels = pixelTableCount;
				mainForm.SetPictureBoxImage (renderImage);
            }
        }

        private void Finish (int result)
        {
            if (result != 0) {
                mainForm.SetPictureBoxImage (null);
            }

            File.Delete (tempFileName);
			if (pixelTable != null) {
				pixelTable.Clear ();
			}
			finishedPixels = 0;

            mainForm.UpdateControlsForEdit ();
        }

        private void SetPixel (int x, int y, double r, double g, double b)
        {
            Color color = Color.FromArgb (255, Convert.ToByte (r * 255), Convert.ToByte (g * 255), Convert.ToByte (b * 255));
			pixelTable.Add (new DictionaryEntry (new Point (x, y), color));
        }
        
        private class RenderWorker
        {
            private BackgroundWorker backgroundWorker = new BackgroundWorker ();

            private String tempFileName;
            private RenderMode renderMode;
            private Settings settings;

            private Action<int> progressCallback;
            private Action<int> finishedCallback;
            private Action<int, int, int, int> startRenderCallback;
            private Action endRenderCallback;
            private Action<int, int, double, double, double> setPixelCallback;

            int result = -1;

            public RenderWorker ()
            {
                backgroundWorker.WorkerReportsProgress = true;
                backgroundWorker.DoWork += new DoWorkEventHandler (DoRayTrace);
                backgroundWorker.ProgressChanged += new ProgressChangedEventHandler (RayTraceProgressChanged);
                backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler (RayTraceCompleted);
            }

            public void Start (RenderMode renderMode, 
                                Settings settings,
                                String tempFileName,
                                Action<int, int, int, int> startRenderCallback,
                                Action endRenderCallback,
                                Action<int> progressCallback,
                                Action<int> finishedCallback,
                                Action<int, int, double, double, double> setPixelCallback)
            {
                this.renderMode = renderMode;
                this.settings = settings;
                this.tempFileName = tempFileName;
                this.startRenderCallback = startRenderCallback;
                this.endRenderCallback = endRenderCallback;
                this.progressCallback = progressCallback;
                this.finishedCallback = finishedCallback;
                this.setPixelCallback = setPixelCallback;
                backgroundWorker.RunWorkerAsync ();
            }

            private void NativeRayTracerStartRenderCallback (int picWidth, int picHeight, int vertexCount, int triangleCount)
            {
                startRenderCallback (picWidth, picHeight, vertexCount, triangleCount);
            }

            private void NativeRayTracerEndRenderCallback ()
            {
                
            }
            
            private void NativeRayTracerProgressCallback (double progress)
            {
                backgroundWorker.ReportProgress ((Int32) (progress * 100.0));
            }

            private void NativeRayTracerSetPixelCallback (int x, int y, double r, double g, double b)
            {
                setPixelCallback (x, y, r, g, b);
            }

            private void DoRayTrace (object sender, DoWorkEventArgs e)
            {
                BackgroundWorker worker = sender as BackgroundWorker;
                Int32 algorithm = 0;
                switch (renderMode) {
                    case RenderMode.RayTraceMode: algorithm = 0; break;
                    case RenderMode.OpenCLTraceMode: algorithm = 1; break;
                    case RenderMode.PathTrace2Mode: algorithm = 2; break;
                }
                result = Win32Functions.Render (
                    algorithm,
                    tempFileName,
                    "",
                    settings.Resolution,
                    NativeRayTracerStartRenderCallback,
                    NativeRayTracerEndRenderCallback,
                    NativeRayTracerProgressCallback,
                    NativeRayTracerSetPixelCallback);
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
    }
}
