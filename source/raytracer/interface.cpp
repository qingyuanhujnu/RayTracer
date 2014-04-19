#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "pathtracer.hpp"
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

int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, ProgressCallback progressCallback)
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
	
	Progress progress (progressCallback);

	std::unique_ptr<Renderer> renderer (new RayTracer (model, camera));
	//std::unique_ptr<Renderer> renderer (new PathTracer (model, camera));
	Renderer::ResultImage resultImage;
	if (DBGERROR (!renderer->Render (parameters, resultImage, progress))) {
		return 3;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 4;
	}	

	return 0;
}
