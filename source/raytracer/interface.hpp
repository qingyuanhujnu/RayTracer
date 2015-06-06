#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	typedef void (*StartRenderCallback) (int picWidth, int picHeight, int vertexCount, int triangleCount);
	typedef void (*EndRenderCallback) ();
	typedef void (*ProgressCallback) (double progress);
	typedef void (*SetPixelCallback) (int x, int y, double r, double g, double b, int picWidth, int picHeight);
	DLLEXPORT int Render (
		int algorithm,
		const wchar_t* configFile,
		const wchar_t* resultFile,
		int sampleNum,
		StartRenderCallback startRenderCallback,
		EndRenderCallback endRenderCallback,
		ProgressCallback progressCallback,
		SetPixelCallback setPixelCallback);

	DLLEXPORT void CancelRender ();
}
