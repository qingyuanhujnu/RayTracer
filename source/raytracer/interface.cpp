#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "pathtracer.hpp"
#include "pathtracer2.hpp"
#include "export.hpp"
#include <memory>

class Progress : public RayTracer::IProgress
{
public:
	Progress (ProgressCallback progressCallback, SetPixelCallback setPixelCallback) :
		progressCallback (progressCallback),
		setPixelCallback (setPixelCallback)
	{

	}

	virtual void OnProgress (double progress) const
	{
		progressCallback (progress);
	}

	virtual void OnPixelReady (int x, int y, double r, double g, double b, int picWidth, int picHeight) const
	{
		setPixelCallback (x, y, r, g, b, picWidth, picHeight);
	}

private:
	ProgressCallback progressCallback;
	SetPixelCallback setPixelCallback;
};

enum RenderMode
{
	RayTraceMode,
	PathTraceMode,
	PathTrace2Mode
};

static int Render (RenderMode mode, const wchar_t* configFile, const wchar_t* resultFile, int sampleNum, ProgressCallback progressCallback, SetPixelCallback setPixelCallback)
{
	if (DBGERROR (configFile == NULL)) {
		return 1;
	}

	if (DBGERROR (resultFile == NULL)) {
		return 1;
	}

	Camera camera;
	Model model;
	Renderer::Parameters parameters;
	if (DBGERROR (!ConfigFile::Read (configFile, parameters, camera, model))) {
		return 2;
	}
	
	std::unique_ptr<Renderer> renderer = NULL;
	if (mode == RayTraceMode) {
		renderer.reset (new RayTracer (model, camera));
	} else if (mode == PathTraceMode) {
		renderer.reset (new PathTracer (model, camera, sampleNum));
	} else if (mode == PathTrace2Mode) {
		renderer.reset (new PathTracer2 (model, camera, sampleNum));
	}
	if (DBGERROR (renderer == NULL)) {
		return 3;
	}

	Progress progress (progressCallback, setPixelCallback);
	Renderer::ResultImage resultImage;
	if (DBGERROR (!renderer->Render (parameters, resultImage, progress))) {
		return 4;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 5;
	}	

	return 0;
}

int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, int sampleNum, ProgressCallback progressCallback, SetPixelCallback setPixelCallback)
{
	return Render (RayTraceMode, configFile, resultFile, sampleNum, progressCallback, setPixelCallback);
}

int PathTrace (const wchar_t* configFile, const wchar_t* resultFile, int sampleNum, ProgressCallback progressCallback, SetPixelCallback setPixelCallback)
{
	return Render (PathTraceMode, configFile, resultFile, sampleNum, progressCallback, setPixelCallback);
}

int PathTrace2 (const wchar_t* configFile, const wchar_t* resultFile, int sampleNum, ProgressCallback progressCallback, SetPixelCallback setPixelCallback)
{
	return Render (PathTrace2Mode, configFile, resultFile, sampleNum, progressCallback, setPixelCallback);
}
