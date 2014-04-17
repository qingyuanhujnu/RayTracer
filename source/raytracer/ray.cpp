#include "ray.hpp"
#include "common.hpp"
#include <algorithm>

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

Ray::ModelIntersection::ModelIntersection () :
	MeshIntersection (),
	mesh (InvalidIndex)
{
}

const Vec3& Ray::GetDirection () const
{
	return direction;
}

Vec3 Ray::GetReflectedDirection (const Vec3& normal) const
{
	double dotProduct = -(normal * direction);
	return direction + (normal * 2.0 * dotProduct);
}

bool Ray::GetSphereIntersection (const Sphere& sphere, ShapeIntersection* intersection) const
{
	Vec3 sphereToRay = origin - sphere.origin;
	double b = (direction * sphereToRay) * 2.0;
	double c = sphereToRay * sphereToRay - sphere.radius * sphere.radius;
	double discriminant = b * b - 4.0 * c;
	if (IsNegative (discriminant)) {
		return false;
	}

	if (intersection != NULL) {
		double distance = 0;
		if (IsZero (discriminant)) {
			distance = -b / 2.0;
		} else {
			double s = sqrt (b * b - 4.0 * c);
			distance = std::min ((-b + s) / 2.0, (-b - s) / 2.0);
		}

		if (IsLengthReached (distance)) {
			return false;
		}

		if (intersection != NULL) {
			intersection->position = origin + direction * distance;
			intersection->distance = distance;
		}
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

bool Ray::GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const
{
	if (!GetSphereIntersection (mesh.GetBoundingSphere (), NULL)) {
		return false;
	}

	bool found = false;
	MeshIntersection minIntersection;

	for (UIndex i = 0; i < mesh.TriangleCount (); i++) {
		const Mesh::Triangle& triangle = mesh.GetTriangle (i);
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
