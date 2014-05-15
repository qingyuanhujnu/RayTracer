#ifndef BOX_HPP
#define BOX_HPP

#include "vec3.hpp"

class Box
{
public:
	Box ();
	Box (const Vec3& min, const Vec3& max);
	Box (const Vec3& origin, double x, double y, double z);
	~Box ();

	Vec3	GetRandomPointInVolume () const;

	Vec3	min;
	Vec3	max;
	Vec3	center;
};

#endif
