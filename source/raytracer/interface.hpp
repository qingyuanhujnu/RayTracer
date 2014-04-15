#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	DLLEXPORT bool __stdcall RayTrace (const wchar_t* configFile, const wchar_t* resultFile, int resolutionX, int resolutionY, double distance);
}
