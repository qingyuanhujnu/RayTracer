#include "sphere.hpp"
#include "common.hpp"
#include "random.hpp"
#include <math.h>

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

static double RandomInRange (double min, double max)
{
    return min + random () * (max - min);
}

bool Sphere::ContainsPoint (const Vec3& point) const
{
	if (IsLowerOrEqual (Distance (point, origin), radius)) {
		return true;
	}
	return false;
}

Vec3 Sphere::GetRandomPointInVolume () const
{
	double x = RandomInRange (-1.0, 1.0);
	double y = RandomInRange (-1.0, 1.0);
	double z = RandomInRange (-1.0, 1.0);

	Vec3 randomDirection =  Vec3 (x, y, z) * (1.0 / sqrt (x * x + y * y + z * z));
	double randomDistance = RandomInRange (0.0, radius);
	
	Vec3 randomPoint = Offset (origin, randomDirection, randomDistance);
	return randomPoint;
}
