#include "sphere.hpp"

Sphere::Sphere () :
	radius (0.0)
{
}

Sphere::Sphere (const Vec3& origin, double radius) :
	origin (origin),
	radius (radius)
{
}

Sphere::~Sphere ()
{
}
