#include "ray.hpp"
#include "common.hpp"

Ray::Ray (const Coord& startPoint, const Coord& rayDirection) :
	origin (startPoint),
	direction (Normalize (rayDirection))
{
}

Ray::TriangleIntersection::TriangleIntersection () :
	distance (INF)
{
}

Ray::MeshIntersection::MeshIntersection () :
	TriangleIntersection (),
	triangle (-1)
{
}

Ray::ModelIntersection::ModelIntersection () :
	MeshIntersection (),
	mesh (-1)
{
}

const Coord& Ray::GetDirection () const
{
	return direction;
}

bool Ray::GetTriangleIntersection (const Coord& v0, const Coord& v1, const Coord& v2, TriangleIntersection* intersection) const
{
	// Moller–Trumbore algorithm

	Coord edgeDir1 = v1 - v0;
	Coord edgeDir2 = v2 - v0;

	Coord p = direction ^ edgeDir2;
	double determinant = edgeDir1 * p;
	if (!IsPositive (determinant)) {
		return false;
	}

	double invDeterminant = 1.0 / determinant;
	Coord originToTriangle = origin - v0;

	double u = (originToTriangle * p) * invDeterminant;
	if (IsLower (u, 0.0) || IsGreater (u, 1.0)) {
		return false;
	}

	Coord q = originToTriangle ^ edgeDir1;
	double v = (direction * q) * invDeterminant;
	if (IsLower (v, 0.0) || IsGreater (u + v, 1.0)) {
		return false;
	}
 
	double distance = (edgeDir2 * q) * invDeterminant;
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

bool Ray::GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const
{
	bool found = false;
	MeshIntersection minIntersection;

	for (UIndex i = 0; i < mesh.TriangleCount (); i++) {
		const Triangle& triangle = mesh.GetTriangle (i);
		const Coord& v0 = mesh.GetVertex (triangle.v0);
		const Coord& v1 = mesh.GetVertex (triangle.v1);
		const Coord& v2 = mesh.GetVertex (triangle.v2);
		
		if (intersection == NULL) {
			if (GetTriangleIntersection (v0, v1, v2, NULL)) {
				found = true;
				break;
			}
		} else {
			Ray::MeshIntersection currentIntersection;
			if (GetTriangleIntersection (v0, v1, v2, &currentIntersection)) {
				if (IsLower (currentIntersection.distance, minIntersection.distance)) {
					minIntersection = currentIntersection;
					minIntersection.triangle = i;
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
	bool found = false;
	ModelIntersection minIntersection;

	for (UIndex i = 0; i < model.MeshCount (); i++) {
		const Mesh& mesh = model.GetMesh (i);
		if (intersection == NULL) {
			if (GetMeshIntersection (mesh, NULL)) {
				found = true;
				break;
			}
		} else {
			ModelIntersection currentIntersection;
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

SectorRay::SectorRay (const Coord& startPoint, const Coord& endPoint) :
	Ray (startPoint, endPoint - startPoint),
	length (Distance (startPoint, endPoint))
{
}

bool SectorRay::IsLengthReached (double currentLength) const
{
	return IsGreater (currentLength, length);
}

InfiniteRay::InfiniteRay (const Coord& startPoint, const Coord& rayDirection) :
	Ray (startPoint, rayDirection)
{
}

bool InfiniteRay::IsLengthReached (double /*currentLength*/) const
{
	return false;
}
