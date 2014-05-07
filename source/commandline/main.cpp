#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

typedef void (*ProgressCallback) (double progress);
typedef void (*PixelReadyCallback) (int x, int y, double r, double g, double b, int picWidth, int picHeight);
typedef bool (*RayTraceFunction) (const wchar_t* configFile, const wchar_t* resultFile, int sampleNum, ProgressCallback progressCallback, PixelReadyCallback pixelReadyCallback);

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

}

static void OnProgressVerbose (double progress)
{
	printf ("%f%%\n", progress * 100.0);
}

static void OnPixelReady (int x, int y, double r, double g, double b, int picWidth, int picHeight)
{

}

void WriteHelp ()
{
	std::wcout << L"parameters:" << std::endl;
	std::wcout << L"    --config       Config file path." << std::endl;
	std::wcout << L"    --result       Result image path." << std::endl;
	std::wcout << L"    --algorithm    Algorithm (raytrace, pathtrace, pathtrace2)." << std::endl;
	std::wcout << L"    --samples	   Sample count for global illumination." << std::endl;
	std::wcout << L"    --verbose      Write progress information." << std::endl;
}

static bool GetParameters (
	int argc, const wchar_t* argv[],
	std::wstring& configFile,
	std::wstring& resultFile,
	std::wstring& algorithm,
	int& sampleNum,
	bool& verbose)
{
	for (int i = 1; i < argc; i++) {
		std::wstring currentParameter (argv[i]);
		if (currentParameter == L"--config") {
			if (i >= argc - 1) {
				return false;
			}
			configFile = argv[i + 1];
			i++;
		} else if (currentParameter == L"--result") {
			if (i >= argc - 1) {
				return false;
			}
			resultFile = argv[i + 1];
			i++;
		} else if (currentParameter == L"--algorithm") {
			if (i >= argc - 1) {
				return false;
			}
			algorithm = argv[i + 1];
			i++;
		} else if (currentParameter == L"--samples") {
			if (i >= argc - 1) {
				return false;
			}
			sampleNum = _wtoi (argv[i + 1]);
			i++;
		} else if (currentParameter == L"--verbose") {
			verbose = true;
		}
	}

	return !configFile.empty () && !resultFile.empty () && !algorithm.empty ();
}

int wmain (int argc, const wchar_t **argv)
{
	LibraryGuard rayTracerModule (L"RayTracer.dll");
	if (rayTracerModule.module == NULL) {
		return 1;
	}

	if (argc != 4) {
	}

	std::wstring configFile;
	std::wstring resultFile;
	std::wstring algorithm;
	int sampleNum = 128;
	bool verbose = false;
	if (!GetParameters (argc, argv, configFile, resultFile, algorithm, sampleNum, verbose)) {
		WriteHelp ();

		// development mode
		RayTraceFunction rayTraceFunction = (RayTraceFunction) GetProcAddress (rayTracerModule.module, "RayTrace");
		if (rayTraceFunction == NULL) {
			return 1;
		}
		rayTraceFunction (L"config01.txt", L"result.png", 32, OnProgressVerbose, OnPixelReady);
		return 1;
	}

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

	time_t begin = time (NULL);
	ProgressCallback progressCallback = OnProgress;
	if (verbose) {
		progressCallback = OnProgressVerbose;
	}
	if (rayTraceFunction (configFile.c_str (), resultFile.c_str (), sampleNum, progressCallback, OnPixelReady) != 0) {
		return 1;
	}
	time_t end = time (NULL);
	
	if (verbose) {
		printf ("elapsed time: %d seconds\n", end - begin);
	}

	return 0;
}
