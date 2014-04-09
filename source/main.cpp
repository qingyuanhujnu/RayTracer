#include <iostream>
#include <string>

#include "raytracer.hpp"
#include "export.hpp"

static void AddQuadrangle (Geometry& geometry, int a, int b, int c, int d, int mat, bool inverted)
{
	if (inverted) {
		geometry.AddTriangle (Triangle (a, c, b, mat));
		geometry.AddTriangle (Triangle (a, d, c, mat));
	} else {
		geometry.AddTriangle (Triangle (a, b, c, mat));
		geometry.AddTriangle (Triangle (a, c, d, mat));
	}
}

static void AddCuboid (Geometry& geometry, double xSize, double ySize, double zSize, const Coord& offset, int material, bool inverted)
{
	double x = xSize / 2.0;
	double y = ySize / 2.0;
	double z = zSize / 2.0;

	int vertexCount = geometry.VertexCount ();
	geometry.AddVertex (offset + Coord (-x, -y, -z)); // 0
	geometry.AddVertex (offset + Coord (x, -y, -z)); // 1
	geometry.AddVertex (offset + Coord (x, -y, z)); // 2
	geometry.AddVertex (offset + Coord (-x, -y, z)); // 3
	geometry.AddVertex (offset + Coord (-x, y, -z)); // 4
	geometry.AddVertex (offset + Coord (x, y, -z)); // 5
	geometry.AddVertex (offset + Coord (x, y, z)); // 6
	geometry.AddVertex (offset + Coord (-x, y, z)); // 7

	AddQuadrangle (geometry, vertexCount + 0, vertexCount + 1, vertexCount + 2, vertexCount + 3, material, inverted);
	AddQuadrangle (geometry, vertexCount + 1, vertexCount + 5, vertexCount + 6, vertexCount + 2, material, inverted);
	AddQuadrangle (geometry, vertexCount + 5, vertexCount + 4, vertexCount + 7, vertexCount + 6, material, inverted);
	AddQuadrangle (geometry, vertexCount + 4, vertexCount + 0, vertexCount + 3, vertexCount + 7, material, inverted);
	AddQuadrangle (geometry, vertexCount + 0, vertexCount + 4, vertexCount + 5, vertexCount + 1, material, inverted); // bottom
	AddQuadrangle (geometry, vertexCount + 3, vertexCount + 2, vertexCount + 6, vertexCount + 7, material, inverted); // top
}

static void ConstructGeometry (Geometry& geometry)
{
	geometry.AddMaterial (Material (Color (1.0, 0.0, 0.0), 0.2, 0.8, 1.0, 0.2));
	geometry.AddMaterial (Material (Color (0.0, 1.0, 0.0), 0.2, 0.8, 0.0, 0.0));
	geometry.AddMaterial (Material (Color (0.0, 0.0, 1.0), 0.2, 0.8, 0.0, 0.0));
	geometry.AddMaterial (Material (Color (1.0, 1.0, 1.0), 0.2, 0.8, 0.0, 0.0));

	AddCuboid (geometry, 4.0, 0.1, 3.0, Coord (0.5, -0.6, 1.5), 0, false);
	AddCuboid (geometry, 1.2, 1.2, 1.8, Coord (-1.0, 1.4, 0.9), 1, false);
	AddCuboid (geometry, 1.2, 1.2, 1.2, Coord (1.0, 1.4, 0.6), 2, false);
	AddCuboid (geometry, 12.0, 12.0, 12.0, Coord (0.0, 0.0, 6.0), 3, true);
}

int main ()
{
	// TODO list:
	// - Generate refraction rays.
	// - Handle vertex normals for curved surfaces.
	// - Create input text file format.
	// - Speed up intersection (first with cached bounding sphere intersection check).
	// - Create user interface.

	Geometry geometry;
	ConstructGeometry (geometry);

	Camera camera (Coord (5.0, 3.0, 4.0), Coord (0.0, 0.0, 0.5), Coord (0.0, 0.0, 1.0), 45.0, 45.0); 
	Light light (Coord (3.0, 3.5, 5.5), Color (1.0, 1.0, 1.0), 1.0, 1.0, 0.0);
	
	RayTracer rayTracer (geometry, camera, light);
	RayTracer::Parameters parameters (400, 400, 1.0);
	RayTracer::ResultImage result;
	rayTracer.Do (parameters, result);
	
	Export::ExportImage (result, L"image/png");
	return 0;
}
