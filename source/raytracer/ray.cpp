#include "ray.hpp"
#include "common.hpp"

Ray::Ray (const Vec3& startPoint, const Vec3& rayDirection) :
	origin (startPoint),
	direction (Normalize (rayDirection))
{
}

Ray::ShapeIntersection::ShapeIntersection () :
	distance (INF)
{
}

Ray::MeshIntersection::MeshIntersection () :
	ShapeIntersection (),
	triangle (InvalidIndex)
{
}

Ray::LightIntersection::LightIntersection () :
	ShapeIntersection (),
	light (InvalidIndex)
{
}

Ray::GeometryIntersection::GeometryIntersection () :
	MeshIntersection (),
	mesh (InvalidIndex)
{
}

Ray::ModelIntersection::ModelIntersection () :
	iSectType (Nothing)
{
}

const Vec3& Ray::GetDirection () const
{
	return direction;
}

bool Ray::GetSphereIntersection (const Sphere& sphere, ShapeIntersection* intersection) const
{
	// from Graphic GEMS

	Vec3 rayToSphere = sphere.origin - origin;
	double v = rayToSphere * direction;

	double discriminant = sphere.radius * sphere.radius - (rayToSphere * rayToSphere - v * v);
	if (IsNegative (discriminant)) {
		return false;
	}

	double distance = v - sqrt (discriminant);
	if (IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = origin + direction * distance;
		intersection->distance = distance;
	}
	return true;
}

enum Quadrant
{
	Left,
	Right,
	Middle
};

static void Vec3ToArray (const Vec3& vec, double arr[3])
{
	arr[0] = vec.x;
	arr[1] = vec.y;
	arr[2] = vec.z;
}

static void ArrayToVec3 (const double arr[3], Vec3& vec)
{
	vec.x = arr[0];
	vec.y = arr[1];
	vec.z = arr[2];
}

bool Ray::GetBoxIntersection (const Box& box, ShapeIntersection* intersection) const
{
	// from Graphic GEMS

	double rayOrigin[3];
	double rayDirection[3];
	double minB[3];
	double maxB[3];

	Vec3ToArray (origin, rayOrigin);
	Vec3ToArray (direction, rayDirection);
	Vec3ToArray (box.min, minB);
	Vec3ToArray (box.max, maxB);

	Quadrant quadrant[3];
	double candidatePlane[3];

	bool originInBox = true;
	for (int i = 0; i < 3; i++) {
		if (IsLower (rayOrigin[i], minB[i])) {
			quadrant[i] = Left;
			candidatePlane[i] = minB[i];
			originInBox = false;
		} else if (IsGreater (rayOrigin[i], maxB[i])) {
			quadrant[i] = Right;
			candidatePlane[i] = maxB[i];
			originInBox = false;
		} else {
			quadrant[i] = Middle;
		}
	}

	if (originInBox) {
		if (intersection != NULL) {
			intersection->position = origin;
			intersection->distance = 0.0;
		}
		return true;
	}

	double maxT[3];
	for (int i = 0; i < 3; i++) {
		if (quadrant[i] != Middle && !IsZero (rayDirection[i])) {
			maxT[i] = (candidatePlane[i] - rayOrigin[i]) / rayDirection[i];
		} else {
			maxT[i] = -1.0;
		}
	}

	int whichPlane = 0;
	for (int i = 1; i < 3; i++) {
		if (IsLower (maxT[whichPlane], maxT[i])) {
			whichPlane = i;
		}
	}

	if (IsNegative (maxT[whichPlane])) {
		return false;
	}

	double xCoord[3];
	for (int i = 0; i < 3; i++) {
		if (whichPlane != i) {
			xCoord[i] = rayOrigin[i] + maxT[whichPlane] * rayDirection[i];
			if (IsLower (xCoord[i], minB[i]) || IsGreater (xCoord[i], maxB[i])) {
				return false;
			}
		} else {
			xCoord[i] = candidatePlane[i];
		}
	}

	Vec3 intersectionCoord;
	ArrayToVec3 (xCoord, intersectionCoord);
	double distance = Distance (origin, intersectionCoord);
	if (IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = intersectionCoord;
		intersection->distance = distance;
	}
	return true;
}

bool Ray::GetTriangleIntersection (const Vec3& v0, const Vec3& v1, const Vec3& v2, ShapeIntersection* intersection) const
{
	// Moller-Trumbore algorithm

	Vec3 edgeDir1 = v1 - v0;
	Vec3 edgeDir2 = v2 - v0;
	Vec3 pVector = direction ^ edgeDir2;
	double determinant = edgeDir1 * pVector;
	if (!IsPositive (determinant)) {
		return false;
	}

	double invDeterminant = 1.0 / determinant;

	Vec3 tVector = origin - v0;
	double u = (tVector * pVector) * invDeterminant;
	if (IsLower (u, 0.0) || IsGreater (u, 1.0)) {
		return false;
	}

	Vec3 qVector = tVector ^ edgeDir1;
	double v = (direction * qVector) * invDeterminant;
	if (IsLower (v, 0.0) || IsGreater (u + v, 1.0)) {
		return false;
	}
 
	double distance = (edgeDir2 * qVector) * invDeterminant;
	if (!IsPositive (distance)) {
		return false;
	}

	if (IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = origin + direction * distance;
		intersection->distance = distance;
	}

	return true;
}

static void GetCandidateTriangles (const Ray& ray, const Octree::Node& node, std::vector<UIndex>& candidateTriangles)
{
	const Box& box = node.GetBox ();
	if (!ray.GetBoxIntersection (box, NULL)) {
		return;
	}

	const std::vector<UIndex>& triangles = node.GetTriangles ();
	for (UIndex i = 0; i < triangles.size (); i++) {
		candidateTriangles.push_back (triangles[i]);
	}

	const std::vector<Octree::Node>& children = node.GetChildren ();
	for (UIndex i = 0; i < children.size (); i++) {
		GetCandidateTriangles (ray, children[i], candidateTriangles);
	}
}

bool Ray::GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const
{
	if (!GetSphereIntersection (mesh.GetBoundingSphere (), NULL)) {
		return false;
	}

	std::vector<UIndex> candidateTriangles;

	static bool useOctree = true;
	if (useOctree) {
		const Octree& octree = mesh.GetOctree ();
		GetCandidateTriangles (*this, octree.GetStartNode (), candidateTriangles);
	} else {
		for (UIndex i = 0; i < mesh.TriangleCount (); i++) {
			candidateTriangles.push_back (i);
		}
	}

	bool found = false;
	MeshIntersection minIntersection;

	for (UIndex i = 0; i < candidateTriangles.size (); i++) {
		UIndex triangleIndex = candidateTriangles[i];
		const Mesh::Triangle& triangle = mesh.GetTriangle (triangleIndex);
		const Vec3& vertex0 = mesh.GetVertex (triangle.vertex0);
		const Vec3& vertex1 = mesh.GetVertex (triangle.vertex1);
		const Vec3& vertex2 = mesh.GetVertex (triangle.vertex2);
		
		if (intersection == NULL) {
			if (GetTriangleIntersection (vertex0, vertex1, vertex2, NULL)) {
				found = true;
				break;
			}
		} else {
			Ray::MeshIntersection currentIntersection;
			if (GetTriangleIntersection (vertex0, vertex1, vertex2, &currentIntersection)) {
				if (IsLower (currentIntersection.distance, minIntersection.distance)) {
					minIntersection = currentIntersection;
					minIntersection.triangle = triangleIndex;
					found = true;
				}
			}
		}
	}

	if (found && intersection != NULL) {
		*intersection = minIntersection;
	}
	return found;
}

bool Ray::GetGeometryIntersection (const Model& model, GeometryIntersection* intersection) const
{
	bool found = false;
	GeometryIntersection minIntersection;

	for (UIndex i = 0; i < model.MeshCount (); i++) {
		const Mesh& mesh = model.GetMesh (i);
		if (intersection == NULL) {
			if (GetMeshIntersection (mesh, NULL)) {
				found = true;
				break;
			}
		} else {
			GeometryIntersection currentIntersection;
			if (GetMeshIntersection (mesh, &currentIntersection)) {
				if (IsLower (currentIntersection.distance, minIntersection.distance)) {
					minIntersection = currentIntersection;
					minIntersection.mesh = i;
					found = true;
				}
			}
		}
	}

	if (found && intersection != NULL) {
		*intersection = minIntersection;
	}
	return found;
}

bool Ray::GetLightIntersection (const Model& model, LightIntersection* intersection) const
{
	bool found = false;
	LightIntersection minIntersection;

	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		Sphere sphere (light.GetPosition (), light.GetRadius ());
		if (intersection == NULL) {
			if (GetSphereIntersection (sphere, NULL)) {
				found = true;
				break;
			}
		}
		else {
			LightIntersection currentIntersection;
			if (GetSphereIntersection (sphere, &currentIntersection)) {
				if (IsLower (currentIntersection.distance, minIntersection.distance)) {
					minIntersection = currentIntersection;
					minIntersection.light = i;
					found = true;
				}
			}
		}
	}

	if (found && intersection != NULL) {
		*intersection = minIntersection;
	}
	return found;
}

bool Ray::GetModelIntersection (const Model& model, ModelIntersection* intersection) const
{
	if (intersection == NULL) {
		return GetGeometryIntersection (model, NULL) || GetLightIntersection (model, NULL);
	}
	else {
		bool wasIsect = false;
		wasIsect |= GetGeometryIntersection (model, &intersection->geometryIntersection);
		wasIsect |= GetLightIntersection (model, &intersection->lightIntersection);

		if (!wasIsect) {
			intersection->iSectType = ModelIntersection::Nothing;
			return false;
		}

		intersection->iSectType = intersection->geometryIntersection.distance < intersection->lightIntersection.distance ?
										ModelIntersection::Geometry :
										ModelIntersection::Light;
	}

	return true;
}

SectorRay::SectorRay (const Vec3& startPoint, const Vec3& endPoint) :
	Ray (startPoint, endPoint - startPoint),
	length (Distance (startPoint, endPoint))
{
}

bool SectorRay::IsLengthReached (double currentLength) const
{
	return IsGreater (currentLength, length);
}

InfiniteRay::InfiniteRay (const Vec3& startPoint, const Vec3& rayDirection) :
	Ray (startPoint, rayDirection)
{
}

bool InfiniteRay::IsLengthReached (double /*currentLength*/) const
{
	return false;
}
