#include "intersection.hpp"
#include <algorithm>

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
	if (sphere.ContainsPoint (rayOrigin)) {
		return true;
	}

	Vec3 sphereToRay;
	FastVecSub (rayOrigin, sphere.origin, sphereToRay);

	const Vec3& rayDirection = ray.GetDirection ();
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
		q = (-b - sqrt (disc)) * 0.5;
	} else {
		q = (-b + sqrt (disc)) * 0.5;
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
		// TODO calculate texCoord
		intersection->texCoord = Vec2 ();
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
			// TODO calculate texcoord
			intersection->texCoord = Vec2 ();
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
		// TODO calculate texcoord
		intersection->texCoord = Vec2 ();
	}
	return true;
}

// Only works if p is inside triangle.
static Vec2 CalculateTexCoord (const Vec3& p, const Triangle& triangle)
{
	Vec3 f1, f2, f3;
	FastVecSub (triangle.v0, p, f1);
	FastVecSub (triangle.v1, p, f2);
	FastVecSub (triangle.v2, p, f3);

	double triangleArea = Length ((triangle.v0 - triangle.v1) ^ (triangle.v0 - triangle.v2));
	double a1 = Length (f2 ^ f3) / triangleArea;
	double a2 = Length (f3 ^ f1) / triangleArea;
	double a3 = Length (f1 ^ f2) / triangleArea;

	double u = triangle.tex0.x * a1 + triangle.tex1.x * a2 + triangle.tex2.x * a3;
	double v = triangle.tex0.y * a1 + triangle.tex1.y * a2 + triangle.tex2.y * a3;

	return Vec2 (u, v);
}

bool Intersection::RayTriangle (const Ray& ray, const Triangle& triangle, FacingMode facing, ShapeIntersection* intersection)
{
	// Moller-Trumbore algorithm

	const Vec3& rayOrigin = ray.GetOrigin ();
	const Vec3& rayDirection = ray.GetDirection ();

	Vec3 edgeDir1, edgeDir2, pVector;
	FastVecSub (triangle.v1, triangle.v0, edgeDir1);
	FastVecSub (triangle.v2, triangle.v0, edgeDir2);
	FastVecMult (rayDirection, edgeDir2, pVector);

	double determinant = edgeDir1 * pVector;
	bool isFrontFacing = IsPositive (determinant);
	if (!isFrontFacing && facing == OnlyFrontFacing) {
		return false;
	}

	double invDeterminant = 1.0 / determinant;

	Vec3 tVector;
	FastVecSub (rayOrigin, triangle.v0, tVector);
	double u = (tVector * pVector) * invDeterminant;
	if (IsLower (u, 0.0) || IsGreater (u, 1.0)) {
		return false;
	}

	Vec3 qVector;
	FastVecMult (tVector, edgeDir1, qVector);
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
		intersection->texCoord = CalculateTexCoord (intersection->position, triangle);
	}

	return true;
}

typedef std::pair<const Octree::Node*, Intersection::ShapeIntersection> OctreeNodeWithIntersection;

static void RayOctree (const Ray& ray, const Octree::Node& node, std::vector<OctreeNodeWithIntersection>& nodesWithIntersections)
{
	if (node.IsEmpty ()) {
		return;
	}

	const Box& box = node.GetBox ();
	Intersection::ShapeIntersection intersection;
	if (!Intersection::RayBox (ray, box, &intersection)) {
		return;
	}

	nodesWithIntersections.push_back (OctreeNodeWithIntersection (&node, intersection));

	const std::vector<Octree::Node>& children = node.GetChildren ();
	for (UIndex i = 0; i < children.size (); i++) {
		RayOctree (ray, children[i], nodesWithIntersections);
	}
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
	auto comparator = [](const OctreeNodeWithIntersection& node1, const OctreeNodeWithIntersection& node2) {
		return node1.second.distance < node2.second.distance;
	};
	std::sort (nodesWithIntersections.begin (), nodesWithIntersections.end (), comparator);

	// Search for nearest triangle intersections.
	Intersection::FacingMode facingMode = Intersection::BothFacing;
	if (!mesh.IsDoubleSided ()) {
		facingMode = Intersection::OnlyFrontFacing;
	}

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
			const Mesh::Vertex& v0 = mesh.GetVertex (triangle.vertex0);
			const Mesh::Vertex& v1 = mesh.GetVertex (triangle.vertex1);
			const Mesh::Vertex& v2 = mesh.GetVertex (triangle.vertex2);
			const Vec2& texCoord0 = mesh.GetTexCoord (triangle.texCoord0);
			const Vec2& texCoord1 = mesh.GetTexCoord (triangle.texCoord1);
			const Vec2& texCoord2 = mesh.GetTexCoord (triangle.texCoord2);

			Intersection::MeshIntersection currentIntersection;
			if (Intersection::RayTriangle (ray, Triangle (v0.pos, v1.pos, v2.pos, texCoord0, texCoord1, texCoord2), facingMode, &currentIntersection)) {
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

static bool RayLightShape (const Ray& ray, const Light& light, Intersection::LightIntersection* intersection)
{
	if (light.GetShape () == Light::SphereShape) {
		const Sphere& sphere = light.GetSphere ();
		return Intersection::RaySphere (ray, sphere, intersection);
	} else if (light.GetShape () == Light::BoxShape) {
		const Box& box = light.GetBox ();
		return Intersection::RayBox (ray, box, intersection);
	}
	return false;
}

bool Intersection::RayLight (const Ray& ray, const Model& model, LightIntersection* intersection)
{
	bool found = false;
	LightIntersection minIntersection;

	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		if (intersection == NULL) {
			if (RayLightShape (ray, light, NULL)) {
				found = true;
				break;
			}
		}
		else {
			LightIntersection currentIntersection;
			if (RayLightShape (ray, light, &currentIntersection)) {
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
