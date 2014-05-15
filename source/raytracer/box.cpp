#include "box.hpp"
#include "random.hpp"

Box::Box ()
{
}

Box::Box (const Vec3& min, const Vec3& max) :
	min (min),
	max (max),
	center ((min + max) / 2.0)
{
}

Box::Box (const Vec3& origin, double x, double y, double z)
{
	Vec3 halfVec (x / 2.0, y / 2.0, z / 2.0);
	min = origin - halfVec;
	max = origin + halfVec;
	center = origin;
}

Box::~Box ()
{
}

Vec3 Box::GetRandomPointInVolume () const
{
	double xDist = RandomInRange (0.0, max.x - min.x);
	double yDist = RandomInRange (0.0, max.y - min.y);
	double zDist = RandomInRange (0.0, max.z - min.z);
	return min + Vec3 (xDist, yDist, zDist);
}
