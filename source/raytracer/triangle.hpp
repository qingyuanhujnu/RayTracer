#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vec3.hpp"
#include "vec2.hpp"

class Triangle
{
public:
	Triangle ();
	Triangle (const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec2& t0, const Vec2& t1, const Vec2& t2);
	~Triangle ();

	Vec3	v0;
	Vec3	v1;
	Vec3	v2;

	Vec2	tex0;
	Vec2	tex1;
	Vec2	tex2;
};

#endif
