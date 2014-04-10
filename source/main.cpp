#include <iostream>
#include <string>

#include "raytracer.hpp"
#include "generator.hpp"
#include "export.hpp"
#include "configfile.hpp"

static void ConstructModel (Model& model)
{
	model.AddMaterial (Material (Color (1.0, 0.0, 0.0), 0.2, 0.8, 1.0, 0.7));
	model.AddMaterial (Material (Color (0.0, 1.0, 0.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (0.0, 0.0, 1.0), 0.2, 0.8, 0.0, 0.0));
	model.AddMaterial (Material (Color (1.0, 1.0, 1.0), 0.2, 0.8, 0.0, 0.0));


	//Generator::GenerateCuboid (model, 4.0, 0.1, 3.0, Coord (0.5, -0.6, 1.5), 0, false);
	//Generator::GenerateCuboid (model, 1.2, 1.2, 1.8, Coord (-1.0, 1.4, 0.9), 1, false);
	//Generator::GenerateCuboid (model, 1.2, 1.2, 1.2, Coord (1.0, 1.4, 0.6), 2, false);

	Generator::GenerateCylinder (model, 1.4, 3.0, 30, Coord (0.5, -1.0, 1.5), 0, false);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.8, Coord (-1.0, 1.5, 0.9), 1, false);
	Generator::GenerateCuboid (model, 1.2, 1.2, 1.2, Coord (1.0, 2.2, 0.6), 2, false);

	Generator::GenerateCuboid (model, 12.0, 12.0, 12.0, Coord (0.0, 0.0, 6.0), 3, true);
}

int main ()
{
	{
		Camera camera;
		Light light;
		Model model;
		ConfigFile::Read (L"config.txt", camera, light, model);	
	}

	Model model;
	ConstructModel (model);

	Camera camera (Coord (5.0, 3.0, 4.0), Coord (0.0, 0.0, 0.5), Coord (0.0, 0.0, 1.0), 45.0, 45.0); 
	Light light (Coord (3.0, 3.5, 5.5), Color (1.0, 1.0, 1.0), 1.0, 1.0, 0.0);
	
	RayTracer rayTracer (model, camera, light);
	RayTracer::Parameters parameters (400, 400, 1.0);
	RayTracer::ResultImage result;
	rayTracer.Do (parameters, result);
	
	Export::ExportImage (result, L"image/png");
	return 0;
}
