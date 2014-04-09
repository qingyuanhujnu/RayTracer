#include "generator.hpp"
#include "common.hpp"

static void AddPolygon (Mesh& mesh, const std::vector<int>& indices, int mat, bool invert)
{
	unsigned int count = indices.size ();
	for (unsigned int i = 0; i < count - 2; i++) {
		if (!invert) {
			mesh.AddTriangle (Triangle (indices[0], indices[(i + 1) % count], indices[(i + 2) % count], mat));
		} else {
			mesh.AddTriangle (Triangle (indices[0], indices[(i + 2) % count], indices[(i + 1) % count], mat));
		}
	}
}

static void AddQuadrangle (Mesh& mesh, int a, int b, int c, int d, int mat, bool invert)
{
	std::vector<int> indices;
	indices.push_back (a);
	indices.push_back (b);
	indices.push_back (c);
	indices.push_back (d);
	AddPolygon (mesh, indices, mat, invert);
}

void Generator::GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, int material, bool invert)
{
	Mesh mesh;

	double x = xSize / 2.0;
	double y = ySize / 2.0;
	double z = zSize / 2.0;

	mesh.AddVertex (offset + Coord (-x, -y, -z));
	mesh.AddVertex (offset + Coord (x, -y, -z));
	mesh.AddVertex (offset + Coord (x, -y, z));
	mesh.AddVertex (offset + Coord (-x, -y, z));
	mesh.AddVertex (offset + Coord (-x, y, -z));
	mesh.AddVertex (offset + Coord (x, y, -z));
	mesh.AddVertex (offset + Coord (x, y, z));
	mesh.AddVertex (offset + Coord (-x, y, z));

	AddQuadrangle (mesh, 0, 1, 2, 3, material, invert);
	AddQuadrangle (mesh, 1, 5, 6, 2, material, invert);
	AddQuadrangle (mesh, 5, 4, 7, 6, material, invert);
	AddQuadrangle (mesh, 4, 0, 3, 7, material, invert);
	AddQuadrangle (mesh, 0, 4, 5, 1, material, invert);
	AddQuadrangle (mesh, 3, 2, 6, 7, material, invert);

	model.AddMesh (mesh);
}

static Coord CylindricalToCartesian (double radius, double height, double theta)
{
	Coord result;
	result.x = radius * cos (theta);
	result.y = radius * sin (theta);
	result.z = height;
	return result;
};

void Generator::GenerateCylinder (Model& model, double radius, double height, int segmentation, const Coord& offset, int material, bool invert)
{
	Mesh mesh;

	double theta = 2.0 * PI;
	double step = 2.0 * PI / segmentation;
	
	for (int i = 0; i < segmentation; i++) {
		mesh.AddVertex (CylindricalToCartesian (radius, height / 2.0, theta));
		mesh.AddVertex (CylindricalToCartesian (radius, -height / 2.0, theta));
		theta -= step;
	}

	for (int i = 0; i < segmentation; i++) {
		int current = 2 * i;
		int next = current + 2;
		if (i == segmentation - 1) {
			next = 0;
		}
		AddQuadrangle (mesh, current, next, next + 1, current + 1, material, invert);
	}

	std::vector<int> topPolygon;
	std::vector<int> bottomPolygon;
	for (int i = 0; i < segmentation; i++) {
		topPolygon.push_back (2 * (segmentation - i - 1));
		bottomPolygon.push_back (2 * i + 1);
	}
	AddPolygon (mesh, topPolygon, material, invert);
	AddPolygon (mesh, bottomPolygon, material, invert);

	model.AddMesh (mesh);
}
