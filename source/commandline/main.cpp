#include <windows.h>
#include <iostream>
#include <string>

typedef bool (*RayTraceFunction) (const wchar_t* configFile, const wchar_t* resultFile, int resolutionX, int resolutionY, double distance);

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

int wmain (int argc, wchar_t **argv)
{
	LibraryGuard rayTracerModule (L"RayTracer.dll");
	if (rayTracerModule.module == NULL) {
		return 1;
	}

	RayTraceFunction rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "RayTrace");
	if (rayTraceFunction == NULL) {
		return 1;
	}

	if (argc != 6) {
		// development mode
		std::wstring configFileName = L"config01.txt";
		rayTraceFunction (L"config01.txt", L"result.png", 400, 400, 1.0);
		return 1;
	}

	int resolutionX = _wtoi (argv[1]);
	if (resolutionX < 0) {
		return 1;
	}

	int resolutionY = _wtoi (argv[2]);
	if (resolutionY < 0) {
		return 1;
	}

	double distance = _wtof (argv[3]);
	if (distance <= 0) {
		return 1;
	}

	std::wstring configFile (argv[4]);
	if (configFile.empty ()) {
		return 1;
	}

	std::wstring resultFile (argv[5]);
	if (resultFile.empty ()) {
		return 1;
	}

	if (rayTraceFunction (configFile.c_str (), resultFile.c_str (), resolutionX, resolutionY, distance) != 0) {
		return 1;
	}
	return 0;
}
