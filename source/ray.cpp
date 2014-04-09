#include "ray.hpp"
#include "common.hpp"

Ray::Intersection::Intersection () :
	distance (INF),
	triangle (-1)
{
}

Ray::Intersection::~Intersection ()
{
}

const Coord& Ray::GetDirection () const
{
	return direction;
}

bool Ray::TriangleIntersection (const Coord& v0, const Coord& v1, const Coord& v2, Coord* intersection) const
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
		*intersection = origin + direction * distance;
	}

	return true;
}

bool Ray::GeometryIntersection (const Mesh& mesh, Intersection* intersection) const
{
	bool found = false;
	Intersection minIntersection;

	for (int i = 0; i < mesh.TriangleCount (); i++) {
		const Triangle& triangle = mesh.GetTriangle (i);
		const Coord& v0 = mesh.GetVertex (triangle.v0);
		const Coord& v1 = mesh.GetVertex (triangle.v1);
		const Coord& v2 = mesh.GetVertex (triangle.v2);
		
		if (intersection == NULL) {
			if (TriangleIntersection (v0, v1, v2, NULL)) {
				found = true;
				break;
			}
		} else {
			Coord currentIntersection;
			if (TriangleIntersection (v0, v1, v2, &currentIntersection)) {
				double currentDistance = Distance (origin, currentIntersection);
				if (IsLower (currentDistance, minIntersection.distance)) {
					minIntersection.position = currentIntersection;
					minIntersection.distance = currentDistance;
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

SectorRay::SectorRay (const Coord& startPoint, const Coord& endPoint)
{
	origin = startPoint;
	direction = Normalize (endPoint - startPoint);
	length = Distance (startPoint, endPoint);
}

bool SectorRay::IsLengthReached (double currentLength) const
{
	return IsGreater (currentLength, length);
}

InfiniteRay::InfiniteRay (const Coord& startPoint, const Coord& rayDirection)
{
	origin = startPoint;
	direction = Normalize (rayDirection);
}

bool InfiniteRay::IsLengthReached (double /*currentLength*/) const
{
	return false;
}
