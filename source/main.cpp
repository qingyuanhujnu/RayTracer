#include <iostream>
#include <string>

#include "raytracer.hpp"
#include "generator.hpp"
#include "export.hpp"
#include "configfile.hpp"
#include "common.hpp"

static void ConstructDefaultModel (Camera& camera, Light& light, Model& model)
{
	camera.Set (Vec3 (5.0, 3.0, 4.0), Vec3 (0.0, 0.0, 0.5), Vec3 (0.0, 0.0, 1.0), 45.0, 45.0); 
	light.Set (Vec3 (3.0, 3.5, 5.5), Color (1.0, 1.0, 1.0), 1.0, 1.0, 0.0);

	model = Model ();

	model.AddMaterial (Material (Color (1.0, 0.0, 0.0), 0.2, 0.8, 1.0, 0.7));
	model.AddMaterial (Material (Color (0.0, 1.0, 0.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (0.0, 0.0, 1.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (1.0, 1.0, 1.0), 0.2, 0.8, 0.0, 0.0));

	Vec3 rotation;
	Generator::GenerateCuboid (model, 4.0, 0.1, 3.0, Vec3 (0.5, -0.6, 1.5), rotation, 0);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.8, Vec3 (-1.0, 1.4, 0.9), rotation, 1);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.2, Vec3 (1.0, 1.4, 0.6), rotation, 2);
	Generator::GenerateInsideOutCuboid (model, 12.0, 12.0, 12.0, Vec3 (0.0, 0.0, 6.0), rotation, 3);
}

int wmain (int argc, wchar_t **argv)
{
	if (argc != 6) {
		// development mode
		std::wstring configFileName = L"config01.txt";

		Camera camera;
		Light light;
		Model model;
		if (DBGERROR (!ConfigFile::Read (configFileName, camera, light, model))) {
			ConstructDefaultModel (camera, light, model);
		}

		if (DBGERROR (!model.Check ())) {
			return 1;
		}

		RayTracer rayTracer (model, camera, light);
		RayTracer::Parameters parameters (400, 400, 1.0);
		RayTracer::ResultImage resultImage;
		rayTracer.Do (parameters, resultImage);
	
		Export::ExportImage (resultImage, L"result.png", L"image/png");
		return 0;
	}

	int resolutionX = _wtoi (argv[1]);
	if (DBGERROR (resolutionX < 0)) {
		return 1;
	}

	int resolutionY = _wtoi (argv[2]);
	if (DBGERROR (resolutionY < 0)) {
		return 1;
	}

	double distance = _wtof (argv[3]);
	if (DBGERROR (!IsPositive (distance))) {
		return 1;
	}

	std::wstring configFile (argv[4]);
	std::wstring resultFile (argv[5]);

	Camera camera;
	Light light;
	Model model;
	if (DBGERROR (!ConfigFile::Read (configFile, camera, light, model))) {
		return 1;
	}

	RayTracer rayTracer (model, camera, light);
	RayTracer::Parameters parameters (resolutionX, resolutionY, distance);
	RayTracer::ResultImage resultImage;
	rayTracer.Do (parameters, resultImage);
	
	if (DBGERROR (!Export::ExportImage (resultImage, resultFile, L"image/png"))) {
		return 1;
	}	

	return 0;
}
