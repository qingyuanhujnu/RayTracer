#include <iostream>
#include <string>

#include "raytracer.hpp"
#include "generator.hpp"
#include "export.hpp"
#include "configfile.hpp"
#include "common.hpp"

static void ConstructDefaultModel (Camera& camera, Light& light, Model& model)
{
	camera.Set (Coord (5.0, 3.0, 4.0), Coord (0.0, 0.0, 0.5), Coord (0.0, 0.0, 1.0), 45.0, 45.0); 
	light.Set (Coord (3.0, 3.5, 5.5), Color (1.0, 1.0, 1.0), 1.0, 1.0, 0.0);

	model = Model ();

	model.AddMaterial (Material (Color (1.0, 0.0, 0.0), 0.2, 0.8, 1.0, 0.7));
	model.AddMaterial (Material (Color (0.0, 1.0, 0.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (0.0, 0.0, 1.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (1.0, 1.0, 1.0), 0.2, 0.8, 0.0, 0.0));

	Generator::GenerateCuboid (model, 4.0, 0.1, 3.0, Coord (0.5, -0.6, 1.5), 0);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.8, Coord (-1.0, 1.4, 0.9), 1);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.2, Coord (1.0, 1.4, 0.6), 2);
	Generator::GenerateInverseCuboid (model, 12.0, 12.0, 12.0, Coord (0.0, 0.0, 6.0), 3);

	//Generator::GenerateCylinder (model, 1.4, 3.0, 30, Coord (0.5, -1.0, 1.5), 0);
	//Generator::GenerateCuboid (model, 1.2, 1.2, 1.8, Coord (-1.0, 1.5, 0.9), 1);
	//Generator::GenerateCuboid (model, 1.2, 1.2, 1.2, Coord (1.0, 2.2, 0.6), 2);
}

int main ()
{
	std::wstring configFileName = L"config01.txt";

	Camera camera;
	Light light;
	Model model;
	if (DBGERROR (!ConfigFile::Read (configFileName, camera, light, model))) {
		ConstructDefaultModel (camera, light, model);
	}

	RayTracer rayTracer (model, camera, light);
	RayTracer::Parameters parameters (400, 400, 1.0);
	RayTracer::ResultImage result;
	rayTracer.Do (parameters, result);
	
	Export::ExportImage (result, L"image/png");
	return 0;
}
