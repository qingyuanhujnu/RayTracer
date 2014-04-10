#include "generator.hpp"
#include "common.hpp"

static void AddPolygon (Mesh& mesh, const std::vector<UIndex>& indices, UIndex material, UIndex curveGroup)
{
	UIndex count = indices.size ();
	for (unsigned int i = 0; i < count - 2; i++) {
		mesh.AddTriangle (Mesh::Triangle (indices[0], indices[(i + 1) % count], indices[(i + 2) % count], material, curveGroup));
	}
}

static void AddQuadrangle (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex d, UIndex material, UIndex curveGroup)
{
	std::vector<UIndex> indices;
	indices.push_back (a);
	indices.push_back (b);
	indices.push_back (c);
	indices.push_back (d);
	AddPolygon (mesh, indices, material, curveGroup);
}

void Generator::GenerateInverseCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, UIndex material)
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

	AddQuadrangle (mesh, 0, 3, 2, 1, material, -1);
	AddQuadrangle (mesh, 1, 2, 6, 5, material, -1);
	AddQuadrangle (mesh, 5, 6, 7, 4, material, -1);
	AddQuadrangle (mesh, 4, 7, 3, 0, material, -1);
	AddQuadrangle (mesh, 0, 1, 5, 4, material, -1);
	AddQuadrangle (mesh, 3, 7, 6, 2, material, -1);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

void Generator::GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, UIndex material)
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

	AddQuadrangle (mesh, 0, 1, 2, 3, material, -1);
	AddQuadrangle (mesh, 1, 5, 6, 2, material, -1);
	AddQuadrangle (mesh, 5, 4, 7, 6, material, -1);
	AddQuadrangle (mesh, 4, 0, 3, 7, material, -1);
	AddQuadrangle (mesh, 0, 4, 5, 1, material, -1);
	AddQuadrangle (mesh, 3, 2, 6, 7, material, -1);

	mesh.Finalize ();
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

void Generator::GenerateCylinder (Model& model, double radius, double height, int segmentation, const Coord& offset, UIndex material)
{
	Mesh mesh;

	double theta = 2.0 * PI;
	double step = 2.0 * PI / segmentation;
	
	for (int i = 0; i < segmentation; i++) {
		mesh.AddVertex (offset + CylindricalToCartesian (radius, height / 2.0, theta));
		mesh.AddVertex (offset + CylindricalToCartesian (radius, -height / 2.0, theta));
		theta -= step;
	}

	for (int i = 0; i < segmentation; i++) {
		int current = 2 * i;
		int next = current + 2;
		if (i == segmentation - 1) {
			next = 0;
		}
		AddQuadrangle (mesh, current, next, next + 1, current + 1, material, 0);
	}

	std::vector<UIndex> topPolygon;
	std::vector<UIndex> bottomPolygon;
	for (int i = 0; i < segmentation; i++) {
		topPolygon.push_back (2 * (segmentation - i - 1));
		bottomPolygon.push_back (2 * i + 1);
	}
	AddPolygon (mesh, topPolygon, material, -1);
	AddPolygon (mesh, bottomPolygon, material, -1);

	mesh.Finalize ();
	model.AddMesh (mesh);
}
