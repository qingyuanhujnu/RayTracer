#include "intersection.hpp"
#include <algorithm>

Intersection::ShapeIntersection::ShapeIntersection () :
	distance (INF)
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
	// from Graphic GEMS

	Vec3 rayToSphere = sphere.origin - ray.GetOrigin ();
	double v = rayToSphere * ray.GetDirection ();

	double discriminant = sphere.radius * sphere.radius - (rayToSphere * rayToSphere - v * v);
	if (IsNegative (discriminant)) {
		return false;
	}

	double distance = v - sqrt (discriminant);
	if (ray.IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = ray.GetOrigin () + ray.GetDirection () * distance;
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

	double rayOrigin[3];
	double rayDirection[3];
	double minB[3];
	double maxB[3];

	Vec3ToArray (ray.GetOrigin (), rayOrigin);
	Vec3ToArray (ray.GetDirection (), rayDirection);
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
			intersection->position = ray.GetOrigin ();
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
	double distance = Distance (ray.GetOrigin (), intersectionCoord);
	if (ray.IsLengthReached (distance)) {
		return false;
	}

	if (intersection != NULL) {
		intersection->position = intersectionCoord;
		intersection->distance = distance;
	}
	return true;
}

bool Intersection::RayTriangle (const Ray& ray, const Vec3& v0, const Vec3& v1, const Vec3& v2, ShapeIntersection* intersection)
{
	// Moller-Trumbore algorithm

	Vec3 edgeDir1 = v1 - v0;
	Vec3 edgeDir2 = v2 - v0;
	Vec3 pVector = ray.GetDirection () ^ edgeDir2;
	double determinant = edgeDir1 * pVector;
	if (!IsPositive (determinant)) {
		return false;
	}

	double invDeterminant = 1.0 / determinant;

	Vec3 tVector = ray.GetOrigin () - v0;
	double u = (tVector * pVector) * invDeterminant;
	if (IsLower (u, 0.0) || IsGreater (u, 1.0)) {
		return false;
	}

	Vec3 qVector = tVector ^ edgeDir1;
	double v = (ray.GetDirection () * qVector) * invDeterminant;
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
		intersection->position = ray.GetOrigin () + ray.GetDirection () * distance;
		intersection->distance = distance;
	}

	return true;
}

bool Intersection::RayOctree (const Ray& ray, const Octree::Node& node, std::vector<OctreeNodeWithIntersection>& nodesWithIntersections)
{
	if (node.IsEmpty ()) {
		return false;
	}

	const Box& box = node.GetBox ();
	Intersection::ShapeIntersection intersection;
	if (!Intersection::RayBox (ray, box, &intersection)) {
		return false;
	}

	nodesWithIntersections.push_back (OctreeNodeWithIntersection (&node, intersection));

	const std::vector<Octree::Node>& children = node.GetChildren ();
	for (UIndex i = 0; i < children.size (); i++) {
		RayOctree (ray, children[i], nodesWithIntersections);
	}

	return true;
}

bool Intersection::RayMesh (const Ray& ray, const Mesh& mesh, MeshIntersection* intersection)
{
	if (!RaySphere (ray, mesh.GetBoundingSphere (), NULL)) {
		return false;
	}

	const Octree& octree = mesh.GetOctree ();
	const Octree::Node& startNode = octree.GetStartNode ();
	std::vector<OctreeNodeWithIntersection> nodesWithIntersections;

	// Get all the nodes and that the ray intersects.
	RayOctree (ray, startNode, nodesWithIntersections);

	// Sort them by distance.
	auto comparator = [](const Intersection::OctreeNodeWithIntersection& node1, const Intersection::OctreeNodeWithIntersection& node2) {
		return node1.second.distance < node2.second.distance;
	};
	std::sort (nodesWithIntersections.begin (), nodesWithIntersections.end (), comparator);

	// Search for nearest triangle intersections.
	bool found = false;
	Intersection::MeshIntersection minIntersection;

	for (auto it = nodesWithIntersections.begin (); it != nodesWithIntersections.end (); ++it) {
		if (found && minIntersection.distance < it->second.distance) {	// The next OctreeNode is behind minIntersection, we can stop.
			break;
		}

		const std::vector<UIndex>& triangles = it->first->GetTriangles ();
		for (auto triangleIt = triangles.begin (); triangleIt != triangles.end (); ++triangleIt) {
			UIndex triangleIndex = *triangleIt;
			const Mesh::Triangle& triangle = mesh.GetTriangle (triangleIndex);
			const Vec3& vertex0 = mesh.GetVertex (triangle.vertex0);
			const Vec3& vertex1 = mesh.GetVertex (triangle.vertex1);
			const Vec3& vertex2 = mesh.GetVertex (triangle.vertex2);

			Intersection::MeshIntersection currentIntersection;
			if (Intersection::RayTriangle (ray, vertex0, vertex1, vertex2, &currentIntersection)) {
				if (intersection == NULL) {
					return true;
				}
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

bool Intersection::RayGeometry (const Ray& ray, const Model& model, GeometryIntersection* intersection)
{
	bool found = false;
	GeometryIntersection minIntersection;

	for (UIndex i = 0; i < model.MeshCount (); i++) {
		const Mesh& mesh = model.GetMesh (i);
		if (intersection == NULL) {
			if (RayMesh (ray, mesh, NULL)) {
				found = true;
				break;
			}
		} else {
			GeometryIntersection currentIntersection;
			if (RayMesh (ray, mesh, &currentIntersection)) {
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

bool Intersection::RayModel (const Ray& ray, const Model& model, ModelIntersection* intersection)
{
	if (intersection == NULL) {
		return RayGeometry (ray, model, NULL) || RayLight (ray, model, NULL);
	}
	else {
		bool wasIsect = false;
		wasIsect |= RayGeometry (ray, model, &intersection->geometryIntersection);
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
