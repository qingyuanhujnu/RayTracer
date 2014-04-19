#include <windows.h>
#include <iostream>
#include <string>

typedef bool (*RayTraceFunction) (const wchar_t* configFile, const wchar_t* resultFile);

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

	if (argc != 3) {
		// development mode
		rayTraceFunction (L"config01.txt", L"result.png");
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

	if (rayTraceFunction (configFile.c_str (), resultFile.c_str ()) != 0) {
		return 1;
	}
	return 0;
}
