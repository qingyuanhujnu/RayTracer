#include "triangle.hpp"

Triangle::Triangle ()
{
}

Triangle::Triangle (const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec2& t0, const Vec2& t1, const Vec2& t2) :
	v0 (v0),
	v1 (v1),
	v2 (v2),
	tex0 (t0),
	tex1 (t1),
	tex2 (t2)
{
}

Triangle::~Triangle ()
{
}
