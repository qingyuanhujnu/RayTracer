#include "ray.hpp"

Ray::Ray (const Vec3& startPoint, const Vec3& rayDirection) :
	origin (startPoint),
	direction (Normalize (rayDirection))
{
}

const Vec3& Ray::GetOrigin () const
{
	return origin;
}

const Vec3& Ray::GetDirection () const
{
	return direction;
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
