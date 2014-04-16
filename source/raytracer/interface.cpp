#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "pathtracer.hpp"
#include "export.hpp"
#include <memory>

int RayTrace (const wchar_t* configFile, const wchar_t* resultFile, int resolutionX, int resolutionY, double distance)
{
	if (DBGERROR (resolutionX < 0)) {
		return 1;
	}

	if (DBGERROR (resolutionY < 0)) {
		return 1;
	}

	if (DBGERROR (!IsPositive (distance))) {
		return 1;
	}

	if (DBGERROR (configFile == NULL)) {
		return 1;
	}

	if (DBGERROR (resultFile == NULL)) {
		return 1;
	}

	Camera camera;
	Light light;
	Model model;
	if (DBGERROR (!ConfigFile::Read (configFile, camera, light, model))) {
		return 2;
	}

	
	std::unique_ptr<Renderer> renderer (new RayTracer (model, camera, light));
	//std::unique_ptr<Renderer> renderer (new PathTracer (model, camera, light));
	Renderer::Parameters parameters (resolutionX, resolutionY, distance);
	Renderer::ResultImage resultImage;
	renderer->Render (parameters, resultImage);	
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 4;
	}	

	return 0;
}
