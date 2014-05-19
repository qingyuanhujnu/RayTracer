#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

typedef void (*StartRenderCallback) (int picWidth, int picHeight, int vertexCount, int triangleCount);
typedef void (*EndRenderCallback) ();
typedef void (*ProgressCallback) (double progress);
typedef void (*SetPixelCallback) (int x, int y, double r, double g, double b);
typedef int (*RenderFunction) (
	int algorithm,
	const wchar_t* configFile,
	const wchar_t* resultFile,
	int sampleNum,
	StartRenderCallback startRenderCallback,
	EndRenderCallback endRenderCallback,
	ProgressCallback progressCallback,
	SetPixelCallback setPixelCallback);

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

static void StartRender (int picWidth, int picHeight, int vertexCount, int triangleCount)
{

}

static void StartRenderVerbose (int picWidth, int picHeight, int vertexCount, int triangleCount)
{
	printf ("image width: %d\n", picWidth);
	printf ("image height: %d\n", picHeight);
	printf ("vertex count: %d\n", vertexCount);
	printf ("triangle count: %d\n", triangleCount);
}

static void EndRender ()
{

}

static void OnProgress (double progress)
{

}

static void OnProgressVerbose (double progress)
{
	printf ("%f%%\n", progress * 100.0);
}

static void OnPixelReady (int x, int y, double r, double g, double b)
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

	RenderFunction renderFunction = (RenderFunction) GetProcAddress (rayTracerModule.module, "Render");
	if (renderFunction == NULL) {
		return 1;
	}

	std::wstring configFile;
	std::wstring resultFile;
	std::wstring algorithm;
	int sampleNum = 128;
	bool verbose = false;
	if (!GetParameters (argc, argv, configFile, resultFile, algorithm, sampleNum, verbose)) {
		WriteHelp ();

		// development mode
		renderFunction (0, L"config01.txt", L"result.png", 32, StartRenderVerbose, EndRender, OnProgressVerbose, OnPixelReady);
		return 1;
	}

	int algorithmVal = 0;
	if (algorithm == L"raytrace") {
		algorithmVal = 0;
	} else if (algorithm == L"pathtrace") {
		algorithmVal = 1;
	} else if (algorithm == L"pathtrace2") {
		algorithmVal = 2;
	}
	if (renderFunction == NULL) {
		return 1;
	}

	time_t begin = time (NULL);
	StartRenderCallback startRenderCallback = StartRender;
	ProgressCallback progressCallback = OnProgress;
	if (verbose) {
		startRenderCallback = StartRenderVerbose;
		progressCallback = OnProgressVerbose;
	}
	if (renderFunction (algorithmVal, configFile.c_str (), resultFile.c_str (), sampleNum, startRenderCallback, EndRender, progressCallback, OnPixelReady) != 0) {
		return 1;
	}
	time_t end = time (NULL);
	
	if (verbose) {
		printf ("elapsed time: %d seconds\n", end - begin);
	}

	return 0;
}
