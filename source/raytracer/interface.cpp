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
	Progress (ProgressCallback progressCallback) :
		RayTracer::IProgress (),
		progressCallback (progressCallback)
	{

	}

	virtual void OnProgress (double progress) const
	{
		progressCallback (progress);
	}

private:
	ProgressCallback progressCallback;
};

enum RenderMode
{
	RayTraceMode,
	PathTraceMode,
	PathTrace2Mode
};

static int Render (RenderMode mode, const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback)
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
		renderer.reset (new PathTracer (model, camera));
	} else if (mode == PathTrace2Mode) {
		renderer.reset (new PathTracer2 (model, camera));
	}
	if (DBGERROR (renderer == NULL)) {
		return 3;
	}

	Progress progress (progressCallback);
	Renderer::ResultImage resultImage;
	if (DBGERROR (!renderer->Render (parameters, resultImage, progress))) {
		return 4;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 5;
	}	

	return 0;
}

int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback)
{
	return Render (RayTraceMode, configFile, resultFile, progressCallback);
}

int PathTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback)
{
	return Render (PathTraceMode, configFile, resultFile, progressCallback);
}

int PathTrace2 (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback)
{
	return Render (PathTrace2Mode, configFile, resultFile, progressCallback);
}
