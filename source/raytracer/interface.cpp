#include "interface.hpp"
#include "random.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "gputracer.hpp"
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
		if (progressCallback != nullptr) {
			progressCallback (progress);
		}
	}

	virtual void OnPixelReady (int x, int y, double r, double g, double b, int picWidth, int picHeight) const
	{
		if (setPixelCallback != nullptr) {
			setPixelCallback (x, y, r, g, b, picWidth, picHeight);
		}
	}

private:
	ProgressCallback progressCallback;
	SetPixelCallback setPixelCallback;
};

enum RenderMode
{
	RayTraceMode,
	OpenCLTraceMode,
	PathTrace2Mode
};

std::unique_ptr<Renderer> renderer = nullptr;

struct ScopeExitResetRenderer {
	~ScopeExitResetRenderer () {
		renderer = nullptr;
	}
};

int Render (
	int algorithm,
	const wchar_t* configFile,
	const wchar_t* resultFile,
	int sampleNum,
	StartRenderCallback startRenderCallback,
	EndRenderCallback endRenderCallback,
	ProgressCallback progressCallback,
	SetPixelCallback setPixelCallback)
{
	RenderMode renderMode = RayTraceMode;
	switch (algorithm) {
		case 0: renderMode = RayTraceMode; break;
		case 1: renderMode = OpenCLTraceMode; break;
		case 2: renderMode = PathTrace2Mode; break;
	}

	if (DBGERROR (configFile == nullptr)) {
		return 1;
	}

	Camera camera;
	Model model;
	Renderer::Parameters parameters;
	if (DBGERROR (!ConfigFile::Read (configFile, parameters, camera, model))) {
		return 2;
	}
	
	if (startRenderCallback != nullptr) {
		startRenderCallback (parameters.GetResolutionX (), parameters.GetResolutionY (), model.VertexCount (), model.TriangleCount ());
	}

	if (renderMode == RayTraceMode) {
		renderer.reset (new RayTracer (model, camera, sampleNum));
	} else if (renderMode == OpenCLTraceMode) {
#ifdef OPENCL
		renderer.reset (new GPUTracer (model, camera, sampleNum));
#endif
	} else if (renderMode == PathTrace2Mode) {
		renderer.reset (new PathTracer2 (model, camera, sampleNum));
	}
	if (DBGERROR (renderer == nullptr)) {
		return 3;
	}
	ScopeExitResetRenderer resetRenderGuard;

	Progress progress (progressCallback, setPixelCallback);
	Renderer::ResultImage resultImage;
	if (DBGERROR (!renderer->Render (parameters, resultImage, progress))) {
		return 4;
	}
	
	if (resultFile != nullptr && resultFile[0] != '\0') {
		if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
			return 5;
		}
	}

	if (endRenderCallback != nullptr) {
		endRenderCallback ();
	}

	return 0;
}

void CancelRender ()
{
	if (renderer != nullptr) {
		renderer->Cancel ();
	}
}
