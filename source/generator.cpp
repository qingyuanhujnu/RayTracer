#include "generator.hpp"

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

void Generator::GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, int material, bool inverted)
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

	AddQuadrangle (mesh, 0, 1, 2, 3, material, inverted);
	AddQuadrangle (mesh, 1, 5, 6, 2, material, inverted);
	AddQuadrangle (mesh, 5, 4, 7, 6, material, inverted);
	AddQuadrangle (mesh, 4, 0, 3, 7, material, inverted);
	AddQuadrangle (mesh, 0, 4, 5, 1, material, inverted);
	AddQuadrangle (mesh, 3, 2, 6, 7, material, inverted);

	model.AddMesh (mesh);
}
