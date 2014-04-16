#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "export.hpp"

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

	RayTracer rayTracer (model, camera, light);
	RayTracer::Parameters parameters (resolutionX, resolutionY, distance);
	RayTracer::ResultImage resultImage;
	if (!rayTracer.Do (parameters, resultImage)) {
		return 3;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 4;
	}	

	return 0;
}
