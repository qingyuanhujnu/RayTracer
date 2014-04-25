#include "triangle.hpp"

Triangle::Triangle ()
{
}

Triangle::Triangle (const Vec3& v0, const Vec3& v1, const Vec3& v2) :
	v0 (v0),
	v1 (v1),
	v2 (v2)
{
}

Triangle::~Triangle ()
{
}
