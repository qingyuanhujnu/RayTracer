#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vec3.hpp"
#include "color.hpp"

class Light
{
public:
	Light ();
	Light (const Vec3& position, const Color& color, double radius);
	~Light ();

	void			Set (const Vec3& position, const Color& color, double radius);

	const Vec3&		GetPosition () const;
	const Color&	GetColor () const;
	double			GetRadius () const;

private:
	Vec3			position;
	Color			color;
	double			radius;
};

#endif
