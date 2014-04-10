#include "mesh.hpp"

#include <algorithm>

Mesh::Triangle::Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex material, UIndex curveGroup) :
	vertex0 (vertex0),
	vertex1 (vertex1),
	vertex2 (vertex2),
	normal0 (InvalidIndex),
	normal1 (InvalidIndex),
	normal2 (InvalidIndex),
	material (material),
	curveGroup (curveGroup)
{

}

Mesh::Triangle::~Triangle ()
{

}

Mesh::Mesh ()
{

}

Mesh::~Mesh ()
{

}

UIndex Mesh::AddVertex (const Coord& coord)
{
	vertices.push_back (coord);
	return vertices.size () - 1;
}

UIndex Mesh::AddTriangle (const Triangle& triangle)
{
	triangles.push_back (triangle);
	triangleNormals.push_back (CalculateTriangleNormal (triangles.size () - 1));
	return triangles.size () - 1;
}

void Mesh::Finalize ()
{
	bool needVertexNormals = false;
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		if (triangle.curveGroup != Mesh::NonCurved) {
			needVertexNormals = true;
			break;
		}
	}

	if (needVertexNormals) {
		CalculateVertexNormals ();
	}
}

UIndex Mesh::VertexCount () const
{
	return vertices.size ();
}

UIndex Mesh::TriangleCount () const
{
	return triangles.size ();
}

const Coord& Mesh::GetVertex (UIndex index) const
{
	return vertices[index];
}

const Mesh::Triangle& Mesh::GetTriangle (UIndex index) const
{
	return triangles[index];
}

static double GetTriangleArea (double a, double b, double c)
{
	double s = (a + b + c) / 2.0;
	return sqrt (s * (s - a) * (s - b) * (s - c));
}

static Coord BarycentricInterpolation (const Coord& vertex0, const Coord& vertex1, const Coord& vertex2,
									   const Coord& value0, const Coord& value1, const Coord& value2,
									   const Coord& interpolationVertex)
{
	double edge0 = Distance (vertex0, vertex1);
	double edge1 = Distance (vertex1, vertex2);
	double edge2 = Distance (vertex2, vertex0);

	double distance0 = Distance (vertex0, interpolationVertex);
	double distance1 = Distance (vertex1, interpolationVertex);
	double distance2 = Distance (vertex2, interpolationVertex);

	double area = GetTriangleArea (edge0, edge1, edge2);
	double area0 = GetTriangleArea (edge0, distance0, distance1);
	double area1 = GetTriangleArea (edge1, distance1, distance2);
	double area2 = GetTriangleArea (edge2, distance0, distance2);

	Coord interpolated = (value0 * area1 + value1 * area2 + value2 * area0) / area;
	return interpolated;
}

Coord Mesh::GetNormal (UIndex index, const Coord& coordinate) const
{
	const Triangle& triangle = triangles[index];
	if (triangle.curveGroup == NonCurved) {
		return triangleNormals[index];
	}

	if (DBGERROR (triangle.normal0 == InvalidIndex || triangle.normal1 == InvalidIndex || triangle.normal2 == InvalidIndex)) {
		return triangleNormals[index];
	}

	const Coord& vertex0 = vertices[triangle.vertex0];
	const Coord& vertex1 = vertices[triangle.vertex1];
	const Coord& vertex2 = vertices[triangle.vertex2];

	const Coord& normal0 = vertexNormals[triangle.normal0];
	const Coord& normal1 = vertexNormals[triangle.normal1];
	const Coord& normal2 = vertexNormals[triangle.normal2];

	Coord normal = BarycentricInterpolation (vertex0, vertex1, vertex2, normal0, normal1, normal2, coordinate);
	return Normalize (normal);
}

static Coord GetAverageNormal (const Mesh& mesh, UIndex baseTriangleIndex, const std::vector<UIndex>& neighbourTriangles, const std::vector<Coord>& triangleNormals)
{
	UIndex baseCurveGroup = mesh.GetTriangle (baseTriangleIndex).curveGroup;

	Coord averageNormal (0.0, 0.0, 0.0);
	int averageNormalCount = 0;

	std::vector<Coord> foundNormals;
	for (UIndex i = 0; i < neighbourTriangles.size (); i++) {
		UIndex currentTriangleIndex = neighbourTriangles[i];
		UIndex currentCurveGroup = mesh.GetTriangle (currentTriangleIndex).curveGroup;
		if (currentCurveGroup == baseCurveGroup) {
			const Coord& currentNormal = triangleNormals[currentTriangleIndex];
			std::vector<Coord>::iterator found = std::find (foundNormals.begin (), foundNormals.end (), currentNormal);
			if (found == foundNormals.end ()) {
				averageNormal = averageNormal + currentNormal;
				averageNormalCount = averageNormalCount + 1;
				foundNormals.push_back (currentNormal);
			}
		}
	}
	return Normalize (averageNormal / averageNormalCount);
}

void Mesh::CalculateVertexNormals ()
{
	vertexNormals.clear ();

	std::vector<std::vector<UIndex>> vertexToTriangle;
	vertexToTriangle.resize (vertices.size ());

	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		vertexToTriangle[triangle.vertex0].push_back (i);
		vertexToTriangle[triangle.vertex1].push_back (i);
		vertexToTriangle[triangle.vertex2].push_back (i);
	}

	for (UIndex i = 0; i < triangles.size (); i++) {
		Triangle& triangle = triangles[i];
		if (triangle.curveGroup != NonCurved) {
			vertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex0], triangleNormals));
			vertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex1], triangleNormals));
			vertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex2], triangleNormals));
			triangle.normal0 = vertexNormals.size () - 3;
			triangle.normal1 = vertexNormals.size () - 2;
			triangle.normal2 = vertexNormals.size () - 1;
		}
	}
}

Coord Mesh::CalculateTriangleNormal (UIndex index)
{
	const Triangle& triangle = triangles[index];

	const Coord& vertex0 = GetVertex (triangle.vertex0);
	const Coord& vertex1 = GetVertex (triangle.vertex1);
	const Coord& vertex2 = GetVertex (triangle.vertex2);

	Coord edgeDir1 = vertex1 - vertex0;
	Coord edgeDir2 = vertex2 - vertex0;
	return Normalize (edgeDir1 ^ edgeDir2);
}
