#include "mesh.hpp"
#include "common.hpp"
#include "average.hpp"

#include <algorithm>

Mesh::Vertex::Vertex (const Vec3& pos, const Vec2& texCoord) :
	pos (pos),
	texCoord (texCoord)
{

}

Mesh::Triangle::Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex material, UIndex curveGroup) :
	vertex0 (vertex0),
	vertex1 (vertex1),
	vertex2 (vertex2),
	normal0 (InvalidIndex),
	normal1 (InvalidIndex),
	normal2 (InvalidIndex),
	material (material),
	curveGroup (curveGroup),
	normalMode (Calculated)
{

}

Mesh::Triangle::Triangle (UIndex vertex0, UIndex vertex1, UIndex vertex2, UIndex normal1, UIndex normal2, UIndex normal3, UIndex material) :
	vertex0 (vertex0),
	vertex1 (vertex1),
	vertex2 (vertex2),
	normal0 (normal1),
	normal1 (normal2),
	normal2 (normal3),
	material (material),
	curveGroup (Mesh::NonCurved),
	normalMode (UserDefined)
{

}

Mesh::Triangle::~Triangle ()
{

}

bool Mesh::Triangle::Check (UIndex materialCount, UIndex vertexCount, UIndex userDefinedVertexNormalCount, UIndex calculatedVertexNormalCount) const
{
	if (DBGERROR (material >= materialCount)) {
		return false;
	}

	if (DBGERROR (vertex0 >= vertexCount || vertex1 >= vertexCount || vertex2 >= vertexCount)) {
		return false;
	}

	if (normalMode == UserDefined) {
		if (DBGERROR (normal0 >= userDefinedVertexNormalCount || normal1 >= userDefinedVertexNormalCount || normal2 >= userDefinedVertexNormalCount)) {
			return false;
		}
	} else if (normalMode == Calculated) {
		if (curveGroup != NonCurved) {
			if (DBGERROR (normal0 >= calculatedVertexNormalCount || normal1 >= calculatedVertexNormalCount || normal2 >= calculatedVertexNormalCount)) {
				return false;
			}
		}
	}

	return true;
}

Mesh::Mesh () :
	doubleSided (true),
	finalized (false)
{

}

Mesh::~Mesh ()
{

}

UIndex Mesh::AddVertex (const Vertex& vertex)
{
	if (DBGERROR (finalized)) {
		return InvalidIndex;
	}
	vertices.push_back (vertex);
	return vertices.size () - 1;
}

void Mesh::SetVertex (UIndex index, const Mesh::Vertex& vertex)
{
	if (DBGERROR (finalized)) {
		return;
	}
	vertices[index] = vertex;
}

UIndex Mesh::AddNormal (const Vec3& normal)
{
	if (DBGERROR (finalized)) {
		return InvalidIndex;
	}
	userDefinedVertexNormals.push_back (Normalize (normal));
	return vertices.size () - 1;
}

UIndex Mesh::AddTriangle (const Triangle& triangle)
{
	if (DBGERROR (finalized)) {
		return InvalidIndex;
	}
	triangles.push_back (triangle);
	return triangles.size () - 1;
}

void Mesh::Transform (const Transformation& transformation)
{
	if (DBGERROR (finalized)) {
		return;
	}
	for (UIndex i = 0; i < vertices.size (); i++) {
		vertices[i].pos = transformation.Apply (vertices[i].pos);
	}
	for (UIndex i = 0; i < userDefinedVertexNormals.size (); i++) {
		userDefinedVertexNormals[i] = transformation.ApplyRotation (userDefinedVertexNormals[i]);
	}
}

void Mesh::SetDoubleSided (bool isDoubleSided)
{
	if (DBGERROR (finalized)) {
		return;
	}
	doubleSided = isDoubleSided;
}

void Mesh::Finalize ()
{
	if (DBGERROR (finalized)) {
		return;
	}

	calculatedTriangleNormals.clear ();
	calculatedVertexNormals.clear ();

	bool needVertexNormals = false;
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		if (triangle.curveGroup != Mesh::NonCurved) {
			needVertexNormals = true;
		}
		calculatedTriangleNormals.push_back (CalculateTriangleNormal (i));
	}

	if (needVertexNormals) {
		CalculateVertexNormals ();
	}

	CalculateBoundingShapes ();
	CalculateOctree ();

	finalized = true;
}

UIndex Mesh::VertexCount () const
{
	return vertices.size ();
}

UIndex Mesh::TriangleCount () const
{
	return triangles.size ();
}

const Mesh::Vertex& Mesh::GetVertex (UIndex index) const
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
	double areaSquare = s * (s - a) * (s - b) * (s - c);
	if (areaSquare < 0.0) {
		return 0.0;
	}
	return sqrt (areaSquare);
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
	if (IsZero (area)) {
		return value0;
	}

	double area0 = GetTriangleArea (edge0, distance0, distance1);
	double area1 = GetTriangleArea (edge1, distance1, distance2);
	double area2 = GetTriangleArea (edge2, distance0, distance2);

	Vec3 interpolated = (value0 * area1 + value1 * area2 + value2 * area0) / area;
	return interpolated;
}

bool Mesh::IsDoubleSided () const
{
	return doubleSided;
}

Vec3 Mesh::GetNormal (UIndex index, const Vec3& coord) const
{
	const Triangle& triangle = triangles[index];
	const Mesh::Vertex& vertex0 = vertices[triangle.vertex0];
	const Mesh::Vertex& vertex1 = vertices[triangle.vertex1];
	const Mesh::Vertex& vertex2 = vertices[triangle.vertex2];

	Vec3 interpolatedNormal;
	if (triangle.normalMode == Triangle::UserDefined) {
		const Vec3& normal0 = userDefinedVertexNormals[triangle.normal0];
		const Vec3& normal1 = userDefinedVertexNormals[triangle.normal1];
		const Vec3& normal2 = userDefinedVertexNormals[triangle.normal2];
		interpolatedNormal = BarycentricInterpolation (vertex0.pos, vertex1.pos, vertex2.pos, normal0, normal1, normal2, coord);
	} else if (triangle.normalMode == Triangle::Calculated){
		if (triangle.curveGroup == NonCurved) {
			return calculatedTriangleNormals[index];
		}

		const Vec3& normal0 = calculatedVertexNormals[triangle.normal0];
		const Vec3& normal1 = calculatedVertexNormals[triangle.normal1];
		const Vec3& normal2 = calculatedVertexNormals[triangle.normal2];
		interpolatedNormal = BarycentricInterpolation (vertex0.pos, vertex1.pos, vertex2.pos, normal0, normal1, normal2, coord);
	}

	return Normalize (interpolatedNormal);
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
	if (DBGERROR (triangles.size () != calculatedTriangleNormals.size ())) {
		return false;
	}

	UIndex vertexCount = vertices.size ();
	UIndex userDefinedVertexNormalCount = userDefinedVertexNormals.size ();
	UIndex calculatedVertexNormalCount = calculatedVertexNormals.size ();
	for (UIndex i = 0; i < triangles.size (); i++) {
		const Triangle& triangle = triangles[i];
		if (DBGERROR (!triangle.Check (materialCount, vertexCount, userDefinedVertexNormalCount, calculatedVertexNormalCount))) {
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

	const Mesh::Vertex& vertex0 = GetVertex (triangle.vertex0);
	const Mesh::Vertex& vertex1 = GetVertex (triangle.vertex1);
	const Mesh::Vertex& vertex2 = GetVertex (triangle.vertex2);

	Vec3 edgeDir1 = vertex1.pos - vertex0.pos;
	Vec3 edgeDir2 = vertex2.pos - vertex0.pos;
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
		if (triangle.normalMode != Triangle::Calculated) {
			continue;
		}
		if (triangle.curveGroup != NonCurved) {
			calculatedVertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex0], calculatedTriangleNormals));
			calculatedVertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex1], calculatedTriangleNormals));
			calculatedVertexNormals.push_back (GetAverageNormal (*this, i, vertexToTriangle[triangle.vertex2], calculatedTriangleNormals));
			triangle.normal0 = calculatedVertexNormals.size () - 3;
			triangle.normal1 = calculatedVertexNormals.size () - 2;
			triangle.normal2 = calculatedVertexNormals.size () - 1;
		}
	}
}

void Mesh::CalculateBoundingShapes ()
{
	Vec3 min (INF, INF, INF);
	Vec3 max (-INF, -INF, -INF);
	for (UIndex i = 0; i < vertices.size (); i++) {
		const Vec3& position = vertices[i].pos;
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
		const Vec3& position = vertices[i].pos;
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
		const Vec3& vertex0 = vertices[triangle.vertex0].pos;
		const Vec3& vertex1 = vertices[triangle.vertex1].pos;
		const Vec3& vertex2 = vertices[triangle.vertex2].pos;
		octree.AddTriangle (i, vertex0, vertex1, vertex2);
	}
}
