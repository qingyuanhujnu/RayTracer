#include <iostream>
#include <string>

#include "raytracer.hpp"
#include "generator.hpp"
#include "export.hpp"
#include "configfile.hpp"

static void ConstructGeometry (Mesh& mesh)
{
	mesh.AddMaterial (Material (Color (1.0, 0.0, 0.0), 0.2, 0.8, 1.0, 0.2));
	mesh.AddMaterial (Material (Color (0.0, 1.0, 0.0), 0.2, 0.8, 0.0, 0.0));
	mesh.AddMaterial (Material (Color (0.0, 0.0, 1.0), 0.2, 0.8, 0.0, 0.0));
	mesh.AddMaterial (Material (Color (1.0, 1.0, 1.0), 0.2, 0.8, 0.0, 0.0));

	Generator::GenerateCuboid (mesh, 4.0, 0.1, 3.0, Coord (0.5, -0.6, 1.5), 0, false);
	Generator::GenerateCuboid (mesh, 1.2, 1.2, 1.8, Coord (-1.0, 1.4, 0.9), 1, false);
	Generator::GenerateCuboid (mesh, 1.2, 1.2, 1.2, Coord (1.0, 1.4, 0.6), 2, false);
	Generator::GenerateCuboid (mesh, 12.0, 12.0, 12.0, Coord (0.0, 0.0, 6.0), 3, true);
}

int main ()
{
	// TODO list:
	// - Generate refraction rays.
	// - Handle vertex normals for curved surfaces.
	// - Create input text file format.
	// - Speed up intersection (first with cached bounding sphere intersection check).
	// - Create user interface.

	{
		Camera camera;
		Light light;
		Mesh mesh;
		ConfigFile::Read (L"config.txt", camera, light, mesh);	
	}

	Mesh mesh;
	ConstructGeometry (mesh);

	Camera camera (Coord (5.0, 3.0, 4.0), Coord (0.0, 0.0, 0.5), Coord (0.0, 0.0, 1.0), 45.0, 45.0); 
	Light light (Coord (3.0, 3.5, 5.5), Color (1.0, 1.0, 1.0), 1.0, 1.0, 0.0);
	
	RayTracer rayTracer (mesh, camera, light);
	RayTracer::Parameters parameters (400, 400, 1.0);
	RayTracer::ResultImage result;
	rayTracer.Do (parameters, result);
	
	Export::ExportImage (result, L"image/png");
	return 0;
}
