#ifndef BOX_HPP
#define BOX_HPP

#include "vec3.hpp"

class Box
{
public:
	Box ();
	Box (const Vec3& min, const Vec3& max);
	~Box ();

	Vec3	min;
	Vec3	max;
};

#endif
