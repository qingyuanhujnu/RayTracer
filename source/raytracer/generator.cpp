#include "generator.hpp"
#include "common.hpp"

static void AddPolygon (Mesh& mesh, const std::vector<UIndex>& vertexIndices, const std::vector<UIndex>& textureVertexIndices, UIndex material, UIndex curveGroup)
{
	UIndex count = vertexIndices.size ();
	for (unsigned int i = 0; i < count - 2; i++) {
		if (textureVertexIndices.empty ()) {
			mesh.AddTriangle (Mesh::Triangle (vertexIndices[0], vertexIndices[(i + 1) % count], vertexIndices[(i + 2) % count], material, curveGroup));
		} else {
			DBGASSERT (vertexIndices.size () == textureVertexIndices.size ());
			mesh.AddTriangle (Mesh::Triangle (vertexIndices[0], vertexIndices[(i + 1) % count], vertexIndices[(i + 2) % count], textureVertexIndices[0], textureVertexIndices[(i + 1) % count], textureVertexIndices[(i + 2) % count], material, curveGroup));
		}
	}
}

static void AddPolygon (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex material, UIndex curveGroup)
{
	mesh.AddTriangle (Mesh::Triangle (a, b, c, material, curveGroup));
}

static void AddTexturedPolygon (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex ta, UIndex tb, UIndex tc, UIndex material, UIndex curveGroup)
{
	mesh.AddTriangle (Mesh::Triangle (a, b, c, ta, tb, tc, material, curveGroup));
}

static void AddPolygon (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex d, UIndex material, UIndex curveGroup)
{
	std::vector<UIndex> vertexIndices;
	std::vector<UIndex> textureVertexIndices;
	vertexIndices.push_back (a);
	vertexIndices.push_back (b);
	vertexIndices.push_back (c);
	vertexIndices.push_back (d);
	AddPolygon (mesh, vertexIndices, textureVertexIndices, material, curveGroup);
}

static void AddTexturedPolygon (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex d, UIndex ta, UIndex tb, UIndex tc, UIndex td, UIndex material, UIndex curveGroup)
{
	std::vector<UIndex> vertexIndices;
	vertexIndices.push_back (a);
	vertexIndices.push_back (b);
	vertexIndices.push_back (c);
	vertexIndices.push_back (d);
	std::vector<UIndex> textureVertexIndices;
	textureVertexIndices.push_back (ta);
	textureVertexIndices.push_back (tb);
	textureVertexIndices.push_back (tc);
	textureVertexIndices.push_back (td);
	AddPolygon (mesh, vertexIndices, textureVertexIndices, material, curveGroup);
}

static void AddPolygon (Mesh& mesh, UIndex a, UIndex b, UIndex c, UIndex d, UIndex e, UIndex material, UIndex curveGroup)
{
	std::vector<UIndex> vertexIndices;
	std::vector<UIndex> textureVertexIndices;
	vertexIndices.push_back (a);
	vertexIndices.push_back (b);
	vertexIndices.push_back (c);
	vertexIndices.push_back (d);
	vertexIndices.push_back (e);
	AddPolygon (mesh, vertexIndices, textureVertexIndices, material, curveGroup);
}

void Generator::GenerateRectangle (Model& model, double xSize, double ySize, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	double x = xSize / 2.0;
	double y = ySize / 2.0;

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, 0.0)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, 0.0)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, 0.0)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, 0.0)));

	AddPolygon (mesh, 0, 1, 2, 3, material, Mesh::NonCurved);

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}


static void GenerateCuboidBase (Mesh& mesh, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex materials[6], Generator::Facing facing, bool doubleSided)
{
	double x = xSize / 2.0;
	double y = ySize / 2.0;
	double z = zSize / 2.0;

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 0.0));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));
	mesh.AddTexCoord (Vec2 (0.0, 1.0));

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, z)));

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, -z)));

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, -z)));

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, z)));

	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, -y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (-x, y, -z)));

	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, -z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, y, z)));
	mesh.AddVertex (Mesh::Vertex (Vec3 (x, -y, z)));

	if (facing == Generator::Inside) {
		AddTexturedPolygon (mesh, 0, 1, 2, 3, 0, 1, 2, 3, materials[0], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 4, 5, 6, 7, 4, 5, 6, 7, materials[1], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 8, 9, 10, 11, 8, 9, 10, 11, materials[2], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 12, 13, 14, 15, 12, 13, 14, 15, materials[3], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 16, 17, 18, 19, 16, 17, 18, 19, materials[4], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 20, 21, 22, 23, 20, 21, 22, 23, materials[5], Mesh::NonCurved);
	} else if (facing == Generator::Outside) {
		AddTexturedPolygon (mesh, 0, 3, 2, 1, 0, 3, 2, 1, materials[0], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 4, 7, 6, 5, 4, 7, 6, 5, materials[1], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 8, 11, 10, 9, 8, 11, 10, 9, materials[2], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 12, 15, 14, 13, 12, 15, 14, 13, materials[3], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 16, 19, 18, 17, 16, 19, 18, 17, materials[4], Mesh::NonCurved);
		AddTexturedPolygon (mesh, 20, 23, 22, 21, 20, 23, 22, 21, materials[5], Mesh::NonCurved);
	} else {
		DBGERROR (true);
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.SetDoubleSided (doubleSided);
	mesh.Finalize ();
}

void Generator::GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	UIndex materials[6];
	for (UIndex i = 0; i < 6; i++) {
		materials[i] = material;
	}
	Mesh mesh;
	GenerateCuboidBase (mesh, xSize, ySize, zSize, offset, rotation, materials, Inside, true);
	model.AddMesh (mesh);
}

void Generator::GenerateRoomBox (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex materials[6])
{
	Mesh mesh;
	GenerateCuboidBase (mesh, xSize, ySize, zSize, offset, rotation, materials, Outside, false);
	model.AddMesh (mesh);
}

void Generator::GenerateCylinder (Model& model, double radius, double height, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	double theta = 2.0 * PI;
	double step = 2.0 * PI / segmentation;
	
	for (int i = 0; i < segmentation; i++) {
		mesh.AddVertex (Mesh::Vertex (CylindricalToCartesian (radius, height / 2.0, theta)));
		mesh.AddVertex (Mesh::Vertex (CylindricalToCartesian (radius, -height / 2.0, theta)));
		mesh.AddTexCoord (Vec2 (theta, height / 2.0));
		mesh.AddTexCoord (Vec2 (theta, -height / 2.0));
		theta -= step;
	}
	
	for (int i = 0; i < segmentation; i++) {
		int current = 2 * i;
		int next = current + 2;
		if (i == segmentation - 1) {
			next = 0;
		}
		AddTexturedPolygon (mesh, current, next, next + 1, current + 1, current, next, next + 1, current + 1, material, 0 /* curvedGroup */);
	}

	std::vector<UIndex> topPolygon;
	std::vector<UIndex> bottomPolygon;
	std::vector<UIndex> topTextureVertices;
	std::vector<UIndex> bottomTextureVertices;
	for (int i = 0; i < segmentation; i++) {
		topPolygon.push_back (2 * (segmentation - i - 1));
		topTextureVertices.push_back (2 * (segmentation - i - 1));
		bottomPolygon.push_back (2 * i + 1);
		bottomTextureVertices.push_back (2 * i + 1);
	}

	AddPolygon (mesh, topPolygon, topTextureVertices, material, Mesh::NonCurved);
	AddPolygon (mesh, bottomPolygon, bottomTextureVertices, material, Mesh::NonCurved);

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

void Generator::GenerateSphere (Model& model, double radius, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	int circle = segmentation * 2;
	double step = PI / segmentation;
	double theta = step;

	UIndex topIndex = mesh.AddVertex (Mesh::Vertex (SphericalToCartesian (radius, 0.0, 0.0)));
	mesh.AddTexCoord (Vec2 (0.0, 0.0));
	for (int i = 1; i < segmentation; i++) {
		double phi = 0;
		for (int j = 0; j < circle; j++) {
			mesh.AddVertex (Mesh::Vertex (SphericalToCartesian (radius, theta, phi)));
			mesh.AddTexCoord (Vec2 (theta, phi));
			phi += step;
		}
		theta += step;
	}
	UIndex bottomIndex = mesh.AddVertex (Mesh::Vertex (SphericalToCartesian (-radius, 0.0, 0.0)));
	mesh.AddTexCoord (Vec2 (1.0, 1.0));

	for (int i = 1; i <= segmentation; i++) {
		if (i == 1) {
			int offset = 1;
			for (int j = 0; j < circle; j++) {
				int current = offset + j;
				int next = current + 1;
				if (j == circle - 1) {
					next = offset;
				}
				AddTexturedPolygon (mesh, current, next, topIndex, current, next, topIndex, material, 0);
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
				AddTexturedPolygon (mesh, current, next, ntop, top, current, next, ntop, top, material, 0);
			}
		} else if (i == segmentation) {
			int offset = (i - 2) * circle + 1;
			for (int j = 0; j < circle; j++) {
				int current = offset + j;
				int next = current + 1;
				if (j == circle - 1) {
					next = offset;
				}
				AddTexturedPolygon (mesh, current, bottomIndex, next, current, bottomIndex, next, material, 0);
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

static Vec3 CoordRotateZ (const Vec3& coord, double angle)
{
	Transformation tr;
	tr.SetRotationZ (angle);
	return tr.Apply (coord);
};

void Generator::GenerateTorus (Model& model, double outerRadius, double innerRadius, int outerSegmentation, int innerSegmentation, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	double theta = 0.0;
	double step = 2.0 * PI / innerSegmentation;
	
	std::vector<Vec3> circle;
	for (int i = 0; i < innerSegmentation; i++) {
		Vec3 coord2D = PolarToCartesian (innerRadius, theta);
		Vec3 coord (coord2D.x + outerRadius, 0.0, coord2D.y);
		circle.push_back (coord);
		theta += step;
	}

	step = (2.0 * PI) / outerSegmentation;
	for (int i = 0; i < outerSegmentation; i++) {
		for (int j = 0; j < innerSegmentation; j++) {
			Vec3 rotated = CoordRotateZ (circle[j], -i * step);
			mesh.AddVertex (rotated);
		}
	}

	for (int i = 0; i < outerSegmentation; i++) {
		for (int j = 0; j < innerSegmentation; j++) {
			UIndex current = i * innerSegmentation + j;
			UIndex next = current + innerSegmentation;
			UIndex top = current + 1;
			UIndex ntop = next + 1;
			
			if (j == innerSegmentation - 1) {
				top = (i * innerSegmentation);
				ntop = (i + 1) * innerSegmentation;
			}

			if (i == outerSegmentation - 1) {
				next = j;
				ntop = j + 1;
				if (j == innerSegmentation - 1) {
					ntop = 0;
				}
			}

			AddPolygon (mesh, current, next, ntop, top, material, 0);
		}
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

void Generator::GenerateSolid (Model& model, SolidType type, double radius, const Vec3& offset, const Vec3& rotation, UIndex material)
{
	Mesh mesh;

	if (type == Tetrahedron) {
		double a = 1.0;

		mesh.AddVertex (Vec3 (+a, +a, +a));
		mesh.AddVertex (Vec3 (-a, -a, +a));
		mesh.AddVertex (Vec3 (-a, +a, -a));
		mesh.AddVertex (Vec3 (+a, -a, -a));

		AddPolygon (mesh, 0, 1, 3, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 2, 1, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 3, 2, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 2, 3, material, Mesh::NonCurved);
	} else if (type == Hexahedron) {
		double a = 1.0;

		mesh.AddVertex (Vec3 (+a, +a, +a));
		mesh.AddVertex (Vec3 (+a, +a, -a));
		mesh.AddVertex (Vec3 (+a, -a, +a));
		mesh.AddVertex (Vec3 (-a, +a, +a));
		mesh.AddVertex (Vec3 (+a, -a, -a));
		mesh.AddVertex (Vec3 (-a, +a, -a));
		mesh.AddVertex (Vec3 (-a, -a, +a));
		mesh.AddVertex (Vec3 (-a, -a, -a));

		AddPolygon (mesh, 0, 1, 5, 3, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 2, 4, 1, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 3, 6, 2, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 4, 7, 5, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 6, 7, 4, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 5, 7, 6, material, Mesh::NonCurved);
	} else if (type == Octahedron) {
		double a = 1.0;
		double b = 0.0;

		mesh.AddVertex (Vec3 (+a, +b, +b));
		mesh.AddVertex (Vec3 (-a, +b, +b));
		mesh.AddVertex (Vec3 (+b, +a, +b));
		mesh.AddVertex (Vec3 (+b, -a, +b));
		mesh.AddVertex (Vec3 (+b, +b, +a));
		mesh.AddVertex (Vec3 (+b, +b, -a));

		AddPolygon (mesh, 0, 2, 4, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 3, 5, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 4, 3, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 5, 2, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 2, 5, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 3, 4, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 4, 2, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 5, 3, material, Mesh::NonCurved);
	} else if (type == Dodecahedron) {
		double a = 1.0;
		double b = 0.0;
		double c = (1.0 + sqrt (5.0)) / 2.0;
		double d = 1.0 / c;

		mesh.AddVertex (Vec3 (+a, +a, +a));
		mesh.AddVertex (Vec3 (+a, +a, -a));
		mesh.AddVertex (Vec3 (+a, -a, +a));
		mesh.AddVertex (Vec3 (-a, +a, +a));
		mesh.AddVertex (Vec3 (+a, -a, -a));
		mesh.AddVertex (Vec3 (-a, +a, -a));
		mesh.AddVertex (Vec3 (-a, -a, +a));
		mesh.AddVertex (Vec3 (-a, -a, -a));
		mesh.AddVertex (Vec3 (+b, +d, +c));
		mesh.AddVertex (Vec3 (+b, +d, -c));
		mesh.AddVertex (Vec3 (+b, -d, +c));
		mesh.AddVertex (Vec3 (+b, -d, -c));
		mesh.AddVertex (Vec3 (+d, +c, +b));
		mesh.AddVertex (Vec3 (+d, -c, +b));
		mesh.AddVertex (Vec3 (-d, +c, +b));
		mesh.AddVertex (Vec3 (-d, -c, +b));
		mesh.AddVertex (Vec3 (+c, +b, +d));
		mesh.AddVertex (Vec3 (-c, +b, +d));
		mesh.AddVertex (Vec3 (+c, +b, -d));
		mesh.AddVertex (Vec3 (-c, +b, -d));

		AddPolygon (mesh, 0, 8, 10, 2, 16, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 16, 18, 1, 12, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 12, 14, 3, 8, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 9, 5, 14, 12, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 18, 4, 11, 9, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 10, 6, 15, 13, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 13, 4, 18, 16, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 14, 5, 19, 17, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 17, 6, 10, 8, material, Mesh::NonCurved);
		AddPolygon (mesh, 4, 13, 15, 7, 11, material, Mesh::NonCurved);
		AddPolygon (mesh, 5, 9, 11, 7, 19, material, Mesh::NonCurved);
		AddPolygon (mesh, 6, 17, 19, 7, 15, material, Mesh::NonCurved);
	} else if (type == Icosahedron) {
		double a = 1.0;
		double b = 0.0;
		double c = (1.0 + sqrt (5.0)) / 2.0;

		mesh.AddVertex (Vec3 (+b, +a, +c));
		mesh.AddVertex (Vec3 (+b, +a, -c));
		mesh.AddVertex (Vec3 (+b, -a, +c));
		mesh.AddVertex (Vec3 (+b, -a, -c));
		mesh.AddVertex (Vec3 (+a, +c, +b));
		mesh.AddVertex (Vec3 (+a, -c, +b));
		mesh.AddVertex (Vec3 (-a, +c, +b));
		mesh.AddVertex (Vec3 (-a, -c, +b));
		mesh.AddVertex (Vec3 (+c, +b, +a));
		mesh.AddVertex (Vec3 (-c, +b, +a));
		mesh.AddVertex (Vec3 (+c, +b, -a));
		mesh.AddVertex (Vec3 (-c, +b, -a));

		AddPolygon (mesh, 0, 2, 8, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 4, 6, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 6, 9, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 8, 4, material, Mesh::NonCurved);
		AddPolygon (mesh, 0, 9, 2, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 3, 11, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 4, 10, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 6, 4, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 10, 3, material, Mesh::NonCurved);
		AddPolygon (mesh, 1, 11, 6, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 5, 8, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 7, 5, material, Mesh::NonCurved);
		AddPolygon (mesh, 2, 9, 7, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 5, 7, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 7, 11, material, Mesh::NonCurved);
		AddPolygon (mesh, 3, 10, 5, material, Mesh::NonCurved);
		AddPolygon (mesh, 4, 8, 10, material, Mesh::NonCurved);
		AddPolygon (mesh, 6, 11, 9, material, Mesh::NonCurved);
		AddPolygon (mesh, 5, 10, 8, material, Mesh::NonCurved);
		AddPolygon (mesh, 7, 9, 11, material, Mesh::NonCurved);
	} else {
		DBGERROR (true);
		return;
	}

	if (DBGERROR (mesh.VertexCount () == 0)) {
		return;
	}

	double currentRadius = Distance (Vec3 (0.0, 0.0, 0.0), mesh.GetVertex (0).pos);
	if (DBGERROR (IsZero (currentRadius))) {
		return;
	}

	double scale = radius / currentRadius;
	for (UIndex i = 0; i < mesh.VertexCount (); i++) {
		mesh.SetVertex (i, mesh.GetVertex (i).pos * scale);
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);
}

void Generator::GenerateMesh (Model& model, const std::vector<Vec3>& vertices, const std::vector<Vec3>& normals, const std::vector<Mesh::Triangle>& triangles, const Vec3& offset, const Vec3& rotation)
{
	Mesh mesh;

	for (UIndex i = 0; i < vertices.size (); i++) {
		mesh.AddVertex (vertices[i]);
	}

	for (UIndex i = 0; i < normals.size (); i++) {
		mesh.AddNormal (normals[i]);
	}

	for (UIndex i = 0; i < triangles.size (); i++) {
		mesh.AddTriangle (triangles[i]);
	}

	Transformation tr;
	tr.AppendRotationXYZ (rotation);
	tr.AppendTranslation (offset);
	mesh.Transform (tr);

	mesh.Finalize ();
	model.AddMesh (mesh);	
}
