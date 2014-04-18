#define DLLEXPORT __declspec(dllexport)

extern "C"
{
	DLLEXPORT int RayTrace (const wchar_t* configFile, const wchar_t* resultFile);
}
