#include "intersection.hpp"

Intersection::ShapeIntersection::ShapeIntersection () :
	distance (INF),
	facing (Unknown)
{
}

Intersection::MeshIntersection::MeshIntersection () :
	ShapeIntersection (),
	triangle (InvalidIndex)
{
}

Intersection::LightIntersection::LightIntersection () :
	ShapeIntersection (),
	light (InvalidIndex)
{
}

Intersection::GeometryIntersection::GeometryIntersection () :
	MeshIntersection (),
	mesh (InvalidIndex)
{
}

Intersection::ModelIntersection::ModelIntersection () :
	iSectType (Nothing)
{
}

bool Intersection::RaySphere (const Ray& ray, const Sphere& sphere, ShapeIntersection* intersection)
{
	const Vec3& rayOrigin = ray.GetOrigin ();
	const Vec3& rayDirection = ray.GetDirection ();

	Vec3 sphereToRay = rayOrigin - sphere.origin;
	double a = rayDirection * rayDirection;
	double b = 2.0 * (sphereToRay * rayDirection);
	double c = sphereToRay * sphereToRay - sphere.radius * sphere.radius;

	double disc = b * b - 4 * a * c;
	if (!IsPositive (disc)) {
		// no intersection
		return false;
	}

	double q = 0.0;
	if (IsNegative (b)) {
		q = (-b - sqrt (disc)) / 2.0;
	} else {
		q = (-b + sqrt (disc)) / 2.0;
	}

	double t0 = q / a;
	double t1 = c / q;
	double maxT = std::max (t0, t1);
	if (IsNegative (maxT)) {
		// object is behind
		return false;
	}

	double distance = 0.0;
	double minT = std::min (t0, t1);
	if (IsNegative (minT)) {
		distance = maxT;
	} else {
		distance = minT;
	}

	if (ray.IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = rayOrigin + distance * rayDirection;
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

bool Intersection::RayBox (const Ray& ray, const Box& box, ShapeIntersection* intersection)
{
	// from Graphic GEMS

	const Vec3& rayOriginVec = ray.GetOrigin ();
	const Vec3& rayDirectionVec = ray.GetDirection ();

	double rayOrigin[3];
	double rayDirection[3];
	double minB[3];
	double maxB[3];

	Vec3ToArray (rayOriginVec, rayOrigin);
	Vec3ToArray (rayDirectionVec, rayDirection);
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
			intersection->position = rayOriginVec;
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
	double distance = Distance (rayOriginVec, intersectionCoord);
	if (ray.IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = intersectionCoord;
		intersection->distance = distance;
	}
	return true;
}

bool Intersection::RayTriangle (const Ray& ray, const Triangle& triangle, FacingMode facing, ShapeIntersection* intersection)
{
	// Moller-Trumbore algorithm

	const Vec3& rayOrigin = ray.GetOrigin ();
	const Vec3& rayDirection = ray.GetDirection ();

	Vec3 edgeDir1 = triangle.v1 - triangle.v0;
	Vec3 edgeDir2 = triangle.v2 - triangle.v0;
	Vec3 pVector = rayDirection ^ edgeDir2;
	double determinant = edgeDir1 * pVector;
	bool isFrontFacing = IsPositive (determinant);
	if (!isFrontFacing && facing == OnlyFrontFacing) {
		return false;
	}

	double invDeterminant = 1.0 / determinant;

	Vec3 tVector = rayOrigin - triangle.v0;
	double u = (tVector * pVector) * invDeterminant;
	if (IsLower (u, 0.0) || IsGreater (u, 1.0)) {
		return false;
	}

	Vec3 qVector = tVector ^ edgeDir1;
	double v = (rayDirection * qVector) * invDeterminant;
	if (IsLower (v, 0.0) || IsGreater (u + v, 1.0)) {
		return false;
	}
 
	double distance = (edgeDir2 * qVector) * invDeterminant;
	if (!IsPositive (distance)) {
		return false;
	}

	if (ray.IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = rayOrigin + rayDirection * distance;
		intersection->distance = distance;
		intersection->facing = isFrontFacing ? Intersection::ShapeIntersection::Front : Intersection::ShapeIntersection::Back;
	}

	return true;
}

static void GetCandidateTriangles (const Ray& ray, const Octree::Node& node, std::vector<UIndex>& candidateTriangles, unsigned int& candidateTriangleCount)
{
	if (node.IsEmpty ()) {
		return;
	}

	const Box& box = node.GetBox ();
	if (!Intersection::RayBox (ray, box, NULL)) {
		return;
	}

	const std::vector<UIndex>& triangles = node.GetTriangles ();
	for (UIndex i = 0; i < triangles.size (); i++) {
		candidateTriangles[candidateTriangleCount] = triangles[i];
		candidateTriangleCount++;
	}

	const std::vector<Octree::Node>& children = node.GetChildren ();
	for (UIndex i = 0; i < children.size (); i++) {
		GetCandidateTriangles (ray, children[i], candidateTriangles, candidateTriangleCount);
	}
}

bool Intersection::RayMesh (const Ray& ray, const Mesh& mesh, FacingMode facing, MeshIntersection* intersection)
{
	if (!RaySphere (ray, mesh.GetBoundingSphere (), NULL)) {
		return false;
	}

	std::vector<UIndex> candidateTriangles;
	unsigned int candidateTriangleCount = 0;
	candidateTriangles.resize (mesh.TriangleCount ());

	static bool useOctree = true;
	if (useOctree) {
		const Octree& octree = mesh.GetOctree ();
		const Octree::Node& startNode = octree.GetStartNode ();
		GetCandidateTriangles (ray, startNode, candidateTriangles, candidateTriangleCount);
	} else {
		candidateTriangleCount = mesh.TriangleCount ();
		for (UIndex i = 0; i < candidateTriangleCount; i++) {
			candidateTriangles[i] = i;
		}
	}

	bool found = false;
	MeshIntersection minIntersection;

	for (UIndex i = 0; i < candidateTriangleCount; i++) {
		UIndex triangleIndex = candidateTriangles[i];
		const Mesh::Triangle& triangle = mesh.GetTriangle (triangleIndex);
		const Vec3& vertex0 = mesh.GetVertex (triangle.vertex0);
		const Vec3& vertex1 = mesh.GetVertex (triangle.vertex1);
		const Vec3& vertex2 = mesh.GetVertex (triangle.vertex2);
		
		if (intersection == NULL) {
			if (RayTriangle (ray, Triangle (vertex0, vertex1, vertex2), facing, NULL)) {
				found = true;
				break;
			}
		} else {
			Intersection::MeshIntersection currentIntersection;
			if (RayTriangle (ray, Triangle (vertex0, vertex1, vertex2), facing, &currentIntersection)) {
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

bool Intersection::RayGeometry (const Ray& ray, const Model& model, FacingMode facing, GeometryIntersection* intersection)
{
	bool found = false;
	GeometryIntersection minIntersection;

	for (UIndex i = 0; i < model.MeshCount (); i++) {
		const Mesh& mesh = model.GetMesh (i);
		if (intersection == NULL) {
			if (RayMesh (ray, mesh, facing, NULL)) {
				found = true;
				break;
			}
		} else {
			GeometryIntersection currentIntersection;
			if (RayMesh (ray, mesh, facing, &currentIntersection)) {
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

bool Intersection::RayLight (const Ray& ray, const Model& model, LightIntersection* intersection)
{
	bool found = false;
	LightIntersection minIntersection;

	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		Sphere sphere (light.GetPosition (), light.GetRadius ());
		if (intersection == NULL) {
			if (RaySphere (ray, sphere, NULL)) {
				found = true;
				break;
			}
		}
		else {
			LightIntersection currentIntersection;
			if (RaySphere (ray, sphere, &currentIntersection)) {
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

bool Intersection::RayModel (const Ray& ray, const Model& model, FacingMode facing, ModelIntersection* intersection)
{
	if (intersection == NULL) {
		return RayGeometry (ray, model, facing, NULL) || RayLight (ray, model, NULL);
	}
	else {
		bool wasIsect = false;
		wasIsect |= RayGeometry (ray, model, facing, &intersection->geometryIntersection);
		wasIsect |= RayLight (ray, model, &intersection->lightIntersection);

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
