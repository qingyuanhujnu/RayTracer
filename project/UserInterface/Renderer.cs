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

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SetPixelCallback(int x, int y, double r, double g, double b, int picWidth, int picHeight);

		[DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
		public static extern int RayTrace (
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
			[MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
            int sampleNum,
            ProgressCallback progressCallback,
            SetPixelCallback setPixelCallback);

        [DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int PathTrace (
            [MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
            [MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
            int sampleNum,
            ProgressCallback progressCallback,
            SetPixelCallback setPixelCallback);

        [DllImport ("RayTracer.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int PathTrace2 (
            [MarshalAsAttribute (UnmanagedType.LPWStr)] string configFile,
            [MarshalAsAttribute (UnmanagedType.LPWStr)] string resultFile,
            int sampleNum,
            ProgressCallback progressCallback,
            SetPixelCallback setPixelCallback);
    }

	class Renderer
    {
        private MainForm mainForm;
        private RenderMode renderMode;
        private String tempFileName;
        private String tempResultFileName;

        private volatile Bitmap renderImage;
        private Bitmap copyImage;

        public enum RenderMode
        {
            RayTraceMode,
            PathTraceMode,
            PathTrace2Mode
        };

        public Renderer (MainForm mainForm, RenderMode renderMode)
		{
            this.mainForm = mainForm;
            this.renderMode = renderMode;
            this.renderImage = null;
            this.copyImage = null;
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

                RenderWorker worker = new RenderWorker ();
                worker.Start (renderMode, tempFileName, tempResultFileName, Progress, Finish, SetPixel);
            } catch {

            }
        }

        private void Progress (int progress)
        {
            mainForm.SetProgressBarValue(progress);

            lock (this)
            {
                if (renderImage != null)
                {
                    if (copyImage != null)
                    {
                        copyImage.Dispose();
                    }
                    copyImage = new Bitmap(renderImage);
                    mainForm.SetPictureBoxImage(copyImage);
                }
            }
        }

        private void Finish (int result)
        {
            if (result == 0) {
                MemoryStream memoryStream = new MemoryStream (File.ReadAllBytes (tempResultFileName));
                Image image = Image.FromStream (memoryStream);
                mainForm.SetPictureBoxImage(image);
            } else {
                mainForm.SetPictureBoxImage (null);
            }

            File.Delete (tempFileName);
            File.Delete (tempResultFileName);

            if (renderImage != null)
            {
                renderImage.Dispose();
            }

            if (copyImage != null)
            {
                copyImage.Dispose();
            }

            mainForm.UpdateControlsForEdit ();
        }

        private void SetPixel (int x, int y, double r, double g, double b, int picWidth, int picHeight)        
        {
            Color color = Color.FromArgb(255,
                                    Convert.ToByte(r * 255),
                                    Convert.ToByte(g * 255),
                                    Convert.ToByte(b * 255));

            lock (this)
            {
                if (renderImage == null || picWidth > renderImage.Width || picHeight > renderImage.Height)
                {
                    renderImage = new Bitmap(picWidth, picHeight);
                }
                renderImage.SetPixel(x, picHeight - 1 - y, color);      // Bitmap.SetPixel is NOT thread safe :(
            }
        }
        
        private class RenderWorker
        {
            private BackgroundWorker backgroundWorker = new BackgroundWorker ();

            private String tempFileName;
            private String tempResultFileName;
            private RenderMode renderMode;

            private Action<int> progressCallback;
            private Action<int> finishedCallback;
            private Action<int, int, double, double, double, int, int> setPixelCallback;

            int result = -1;

            public RenderWorker ()
            {
                backgroundWorker.WorkerReportsProgress = true;
                backgroundWorker.DoWork += new DoWorkEventHandler (DoRayTrace);
                backgroundWorker.ProgressChanged += new ProgressChangedEventHandler (RayTraceProgressChanged);
                backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler (RayTraceCompleted);
            }

            public void Start (RenderMode renderMode, 
                                String tempFileName,
                                String tempResultFileName,
                                Action<int> progressCallback,
                                Action<int> finishedCallback,
                                Action<int, int, double, double, double, int, int> setPixelCallback)
            {
                this.renderMode = renderMode;
                this.tempFileName = tempFileName;
                this.tempResultFileName = tempResultFileName;
                this.progressCallback = progressCallback;
                this.finishedCallback = finishedCallback;
                this.setPixelCallback = setPixelCallback;
                backgroundWorker.RunWorkerAsync ();
            }

            private void NativeRayTracerProgressCallback (double progress)
            {
                backgroundWorker.ReportProgress ((Int32)(progress * 100.0));
            }

            private void NativeRayTracerSetPixelCallback (int x, int y, double r, double g, double b, int picWidth, int picHeight)
            {
                setPixelCallback(x, y, r, g, b, picWidth, picHeight);
            }

            private void DoRayTrace (object sender, DoWorkEventArgs e)
            {
                BackgroundWorker worker = sender as BackgroundWorker;
                if (renderMode == RenderMode.RayTraceMode) {
                    result = Win32Functions.RayTrace (tempFileName, tempResultFileName, 32, NativeRayTracerProgressCallback, NativeRayTracerSetPixelCallback);
                } else if (renderMode == RenderMode.PathTraceMode) {
                    result = Win32Functions.PathTrace(tempFileName, tempResultFileName, 32, NativeRayTracerProgressCallback, NativeRayTracerSetPixelCallback);
                } else if (renderMode == RenderMode.PathTrace2Mode) {
                    result = Win32Functions.PathTrace2(tempFileName, tempResultFileName, 32, NativeRayTracerProgressCallback, NativeRayTracerSetPixelCallback);
                }
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
