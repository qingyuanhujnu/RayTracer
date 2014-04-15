#include "interface.hpp"
#include "configfile.hpp"
#include "raytracer.hpp"
#include "export.hpp"

bool RayTrace (const wchar_t* configFile, const wchar_t* resultFile, int resolutionX, int resolutionY, double distance)
{
	if (DBGERROR (resolutionX < 0)) {
		return false;
	}

	if (DBGERROR (resolutionY < 0)) {
		return false;
	}

	if (DBGERROR (!IsPositive (distance))) {
		return false;
	}

	if (DBGERROR (configFile == NULL)) {
		return false;
	}

	if (DBGERROR (resultFile == NULL)) {
		return false;
	}

	Camera camera;
	Light light;
	Model model;
	if (DBGERROR (!ConfigFile::Read (configFile, camera, light, model))) {
		return false;
	}

	RayTracer rayTracer (model, camera, light);
	RayTracer::Parameters parameters (resolutionX, resolutionY, distance);
	RayTracer::ResultImage resultImage;
	if (!rayTracer.Do (parameters, resultImage)) {
		return false;
	}
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return false;
	}	

	return true;
}
