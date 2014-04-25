#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "vec3.hpp"

class Triangle
{
public:
	Triangle ();
	Triangle (const Vec3& v0, const Vec3& v1, const Vec3& v2);
	~Triangle ();

	Vec3	v0;
	Vec3	v1;
	Vec3	v2;
};

#endif
