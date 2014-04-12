#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "vec3.hpp"

class Sphere
{
public:
	Sphere ();
	Sphere (const Vec3& origin, double radius);
	~Sphere ();

	Vec3	origin;
	double	radius;
};

#endif
