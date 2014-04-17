#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "pathtracer.hpp"
#include "export.hpp"
#include <memory>

int RayTrace (const wchar_t* configFile, const wchar_t* resultFile)
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
	
	std::unique_ptr<Renderer> renderer (new RayTracer (model, camera));
	//std::unique_ptr<Renderer> renderer (new PathTracer (model, camera));
	Renderer::ResultImage resultImage;
	if (DBGERROR (!renderer->Render (parameters, resultImage))) {
		return 3;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 4;
	}	

	return 0;
}
