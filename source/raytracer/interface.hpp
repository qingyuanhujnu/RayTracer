#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	typedef void (*ProgressCallback) (double progress);
	typedef void (*SetPixelCallback) (int x, int y, double r, double g, double b, int picWidth, int picHeight);
	DLLEXPORT int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback, SetPixelCallback setPixelCallback);
	DLLEXPORT int PathTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback, SetPixelCallback setPixelCallback);
	DLLEXPORT int PathTrace2 (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback, SetPixelCallback setPixelCallback);
}
