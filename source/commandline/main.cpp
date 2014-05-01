#include <windows.h>
#include <iostream>
#include <string>

typedef void (*ProgressCallback) (double progress);
typedef void (*PixelReadyCallback) (int x, int y, double r, double g, double b, int picWidth, int picHeight);
typedef bool (*RayTraceFunction) (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback, PixelReadyCallback pixelReadyCallback);

class LibraryGuard
{
public:
	LibraryGuard (const wchar_t* name)
	{
		module = LoadLibrary (name);
	}

	~LibraryGuard ()
	{
		FreeLibrary (module);
	}

	HMODULE module;
};

static void OnProgress (double progress)
{
	//printf ("%f\n", progress);
}

static void OnPixelReady (int x, int y, double r, double g, double b, int picWidth, int picHeight)
{

}

int wmain (int argc, wchar_t **argv)
{
	LibraryGuard rayTracerModule (L"RayTracer.dll");
	if (rayTracerModule.module == NULL) {
		return 1;
	}

	if (argc != 4) {
		// development mode
		RayTraceFunction rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "RayTrace");
		if (rayTraceFunction == NULL) {
			return 1;
		}
		rayTraceFunction (L"config01.txt", L"result.png", OnProgress, OnPixelReady);
		return 1;
	}

	std::wstring configFile (argv[1]);
	if (configFile.empty ()) {
		return 1;
	}

	std::wstring resultFile (argv[2]);
	if (resultFile.empty ()) {
		return 1;
	}

	std::wstring algorithm (argv[3]);

	RayTraceFunction rayTraceFunction = NULL;
	if (algorithm == L"raytrace") {
		rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "RayTrace");
	} else if (algorithm == L"pathtrace") {
		rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "PathTrace");
	} else if (algorithm == L"pathtrace2") {
		rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "PathTrace2");
	}
	if (rayTraceFunction == NULL) {
		return 1;
	}

	if (rayTraceFunction (configFile.c_str (), resultFile.c_str (), OnProgress, OnPixelReady) != 0) {
		return 1;
	}
	return 0;
}
