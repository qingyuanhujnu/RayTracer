#include "box.hpp"

Box::Box ()
{
}

Box::Box (const Vec3& min, const Vec3& max) :
	min (min),
	max (max)
{
}

Box::~Box ()
{
}
