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

static void AddTriangle (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex material, UIndex curveGroup)
{
	mesh.AddTriangle (Mesh::Triangle (a, b, c, material, curveGroup));
}

static void GenerateCuboidBase (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material, Generator::Facing facing)
{
	Mesh mesh;

	double x = xSize / 2.0;
	double y = ySize / 2.0;
	double z = zSize / 2.0;

	mesh.AddVertex (Vec3 (-x, -y, -z));
	mesh.AddVertex (Vec3 (x, -y, -z));
	mesh.AddVertex (Vec3 (x, -y, z));
	mesh.AddVertex (Vec3 (-x, -y, z));
	mesh.AddVertex (Vec3 (-x, y, -z));
	mesh.AddVertex (Vec3 (x, y, -z));
	mesh.AddVertex (Vec3 (x, y, z));
	mesh.AddVertex (Vec3 (-x, y, z));

	if (facing == Generator::Inside) {
		AddQuadrangle (mesh, 0, 1, 2, 3, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 1, 5, 6, 2, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 5, 4, 7, 6, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 4, 0, 3, 7, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 0, 4, 5, 1, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 3, 2, 6, 7, material, Mesh::NonCurved);
	} else if (facing == Generator::Outside) {
		AddQuadrangle (mesh, 0, 3, 2, 1, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 1, 2, 6, 5, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 5, 6, 7, 4, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 4, 7, 3, 0, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 0, 1, 5, 4, material, Mesh::NonCurved);
		AddQuadrangle (mesh, 3, 7, 6, 2, material, Mesh::NonCurved);
	} else {
		DBGERROR (true);
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

void Generator::GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	GenerateCuboidBase (model, xSize, ySize, zSize, offset, rotation, material, Inside);
}

void Generator::GenerateInsideOutCuboid (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	GenerateCuboidBase (model, xSize, ySize, zSize, offset, rotation, material, Outside);
}

static Vec3 CylindricalToCartesian (double radius, double height, double theta)
{
	Vec3 result;
	result.x = radius * cos (theta);
	result.y = radius * sin (theta);
	result.z = height;
	return result;
};

void Generator::GenerateCylinder (Model& model, double radius, double height, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material)
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
		AddQuadrangle (mesh, current, next, next + 1, current + 1, material, 0 /* curvedGroup */);
	}

	std::vector<UIndex> topPolygon;
	std::vector<UIndex> bottomPolygon;
	for (int i = 0; i < segmentation; i++) {
		topPolygon.push_back (2 * (segmentation - i - 1));
		bottomPolygon.push_back (2 * i + 1);
	}

	AddPolygon (mesh, topPolygon, material, Mesh::NonCurved);
	AddPolygon (mesh, bottomPolygon, material, Mesh::NonCurved);

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

static Vec3 SphericalToCartesian (double radius, double theta, double phi)
{
	Vec3 result;
	result.x = radius * sin (theta) * cos (phi);
	result.y = radius * sin (theta) * sin (phi);
	result.z = radius * cos (theta);
	return result;
};

void Generator::GenerateSphere (Model& model, double radius, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	int circle = segmentation * 2;
	double step = PI / segmentation;
	double theta = step;

	UIndex topIndex = mesh.AddVertex (SphericalToCartesian (radius, 0.0, 0.0));
	for (int i = 1; i < segmentation; i++) {
		double phi = 0;
		for (int j = 0; j < circle; j++) {
			mesh.AddVertex (SphericalToCartesian (radius, theta, phi));
			phi += step;
		}
		theta += step;
	}
	UIndex bottomIndex = mesh.AddVertex (SphericalToCartesian (-radius, 0.0, 0.0));

	for (int i = 1; i <= segmentation; i++) {
		if (i == 1) {
			int offset = 1;
			for (int j = 0; j < circle; j++) {
				int current = offset + j;
				int next = current + 1;
				if (j == circle - 1) {
					next = offset;
				}
				AddTriangle (mesh, current, next, topIndex, material, 0);
			}
		} else if (i < segmentation) {
			int offset = (i - 1) * circle + 1;
			for (int j = 0; j < circle; j++) {
				int current = offset + j;
				int next = current + 1;
				int top = current - circle;
				int ntop = top + 1;
				if (j == circle - 1) {
					next = offset;
					ntop = offset - circle;
				}
				AddQuadrangle (mesh, current, next, ntop, top, material, 0);
			}
		} else if (i == segmentation) {
			int offset = (i - 2) * circle + 1;
			for (int j = 0; j < circle; j++) {
				int current = offset + j;
				int next = current + 1;
				if (j == circle - 1) {
					next = offset;
				}
				AddTriangle (mesh, current, bottomIndex, next, material, 0);
			}
		}
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}
