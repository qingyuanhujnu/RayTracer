#include "mesh.hpp"
#include "common.hpp"
#include "average.hpp"

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

bool Mesh::Triangle::Check (UIndex materialCount, UIndex vertexCount, UIndex vertexNormalCount) const
{
	if (DBGERROR (material >= materialCount)) {
		return false;
	}

	if (DBGERROR (vertex0 >= vertexCount || vertex1 >= vertexCount || vertex2 >= vertexCount)) {
		return false;
	}

	if (curveGroup != NonCurved) {
		if (DBGERROR (normal0 >= vertexNormalCount || normal1 >= vertexNormalCount || normal2 >= vertexNormalCount)) {
			return false;
		}
	}

	return true;
}

Mesh::Mesh ()
{

}

Mesh::~Mesh ()
{

}

UIndex Mesh::AddVertex (const Vec3& position)
{
	vertices.push_back (position);
	return vertices.size () - 1;
}

void Mesh::SetVertex (UIndex index, const Vec3& position)
{
	vertices[index] = position;
}

UIndex Mesh::AddTriangle (const Triangle& triangle)
{
	triangles.push_back (triangle);
	return triangles.size () - 1;
}

void Mesh::Transform (const Transformation& transformation)
{
	for (UIndex i = 0; i < vertices.size (); i++) {
		vertices[i] = transformation.Apply (vertices[i]);
	}
}

void Mesh::Finalize ()
{
	triangleNormals.clear ();
	vertexNormals.clear ();

	bool needVertexNormals = false;
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		if (triangle.curveGroup != Mesh::NonCurved) {
			needVertexNormals = true;
		}
		triangleNormals.push_back (CalculateTriangleNormal (i));
	}

	if (needVertexNormals) {
		CalculateVertexNormals ();
	}

	CalculateBoundingShapes ();
	CalculateOctree ();
}

UIndex Mesh::VertexCount () const
{
	return vertices.size ();
}

UIndex Mesh::TriangleCount () const
{
	return triangles.size ();
}

const Vec3& Mesh::GetVertex (UIndex index) const
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

static Vec3 BarycentricInterpolation (const Vec3& vertex0, const Vec3& vertex1, const Vec3& vertex2,
									   const Vec3& value0, const Vec3& value1, const Vec3& value2,
									   const Vec3& interpolationVertex)
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

	Vec3 interpolated = (value0 * area1 + value1 * area2 + value2 * area0) / area;
	return interpolated;
}

Vec3 Mesh::GetNormal (UIndex index, const Vec3& coord) const
{
	const Triangle& triangle = triangles[index];
	if (triangle.curveGroup == NonCurved) {
		return triangleNormals[index];
	}

	if (DBGERROR (triangle.normal0 == InvalidIndex || triangle.normal1 == InvalidIndex || triangle.normal2 == InvalidIndex)) {
		return triangleNormals[index];
	}

	const Vec3& vertex0 = vertices[triangle.vertex0];
	const Vec3& vertex1 = vertices[triangle.vertex1];
	const Vec3& vertex2 = vertices[triangle.vertex2];

	const Vec3& normal0 = vertexNormals[triangle.normal0];
	const Vec3& normal1 = vertexNormals[triangle.normal1];
	const Vec3& normal2 = vertexNormals[triangle.normal2];

	Vec3 normal = BarycentricInterpolation (vertex0, vertex1, vertex2, normal0, normal1, normal2, coord);
	return Normalize (normal);
}

const Box& Mesh::GetBoundingBox () const
{
	return boundingBox;
}

const Sphere& Mesh::GetBoundingSphere () const
{
	return boundingSphere;
}

const Octree& Mesh::GetOctree () const
{
	return octree;
}

bool Mesh::Check (UIndex materialCount) const
{
	if (DBGERROR (triangles.size () != triangleNormals.size ())) {
		return false;
	}

	UIndex vertexCount = vertices.size ();
	UIndex vertexNormalCount = vertexNormals.size ();
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		if (DBGERROR (!triangle.Check (materialCount, vertexCount, vertexNormalCount))) {
			return false;
		}
	}
	return true;
}

static Vec3 GetAverageNormal (const Mesh& mesh, UIndex baseTriangleIndex, const std::vector<UIndex>& neighbourTriangles, const std::vector<Vec3>& triangleNormals)
{
	UIndex baseCurveGroup = mesh.GetTriangle (baseTriangleIndex).curveGroup;

	Average<Vec3> averageNormal;
	std::vector<Vec3> foundNormals;
	for (UIndex i = 0; i < neighbourTriangles.size (); i++) {
		UIndex currentTriangleIndex = neighbourTriangles[i];
		UIndex currentCurveGroup = mesh.GetTriangle (currentTriangleIndex).curveGroup;
		if (currentCurveGroup == baseCurveGroup) {
			const Vec3& currentNormal = triangleNormals[currentTriangleIndex];
			std::vector<Vec3>::iterator found = std::find (foundNormals.begin (), foundNormals.end (), currentNormal);
			if (found == foundNormals.end ()) {
				averageNormal.Add (currentNormal);
				foundNormals.push_back (currentNormal);
			}
		}
	}
	return Normalize (averageNormal.Get ());
}

Vec3 Mesh::CalculateTriangleNormal (UIndex index)
{
	const Triangle& triangle = triangles[index];

	const Vec3& vertex0 = GetVertex (triangle.vertex0);
	const Vec3& vertex1 = GetVertex (triangle.vertex1);
	const Vec3& vertex2 = GetVertex (triangle.vertex2);

	Vec3 edgeDir1 = vertex1 - vertex0;
	Vec3 edgeDir2 = vertex2 - vertex0;
	return Normalize (edgeDir1 ^ edgeDir2);
}

void Mesh::CalculateVertexNormals ()
{
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

void Mesh::CalculateBoundingShapes ()
{
	Vec3 min (INF, INF, INF);
	Vec3 max (-INF, -INF, -INF);
	for (UIndex i = 0; i < vertices.size (); i++) {
		const Vec3& position = vertices[i];
		if (position.x < min.x) { min.x = position.x; };
		if (position.y < min.y) { min.y = position.y; };
		if (position.z < min.z) { min.z = position.z; };
		if (position.x > max.x) { max.x = position.x; };
		if (position.y > max.y) { max.y = position.y; };
		if (position.z > max.z) { max.z = position.z; };
	}
	boundingBox.min = min;
	boundingBox.max = max;
	boundingSphere.origin = (min + max) / 2.0;

	double maxDistance = -INF;
	for (UIndex i = 0; i < vertices.size (); i++) {
		const Vec3& position = vertices[i];
		double currentDistance = Distance (position, boundingSphere.origin);
		if (currentDistance > maxDistance) {
			maxDistance = currentDistance;
		}
	}
	boundingSphere.radius = maxDistance;
}

void Mesh::CalculateOctree ()
{
	octree.SetBox (boundingBox);
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		const Vec3& vertex0 = vertices[triangle.vertex0];
		const Vec3& vertex1 = vertices[triangle.vertex1];
		const Vec3& vertex2 = vertices[triangle.vertex2];
		octree.AddTriangle (i, vertex0, vertex1, vertex2);
	}
}
