#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	typedef void (*ProgressCallback) (double progress);
	DLLEXPORT int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback);
}
