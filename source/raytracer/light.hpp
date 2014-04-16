#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vec3.hpp"
#include "color.hpp"

class Light
{
public:
	Light ();
	Light (const Vec3& position, const Color& color);
	~Light ();

	void			Set (const Vec3& position, const Color& color);

	const Vec3&		GetPosition () const;
	const Color&	GetColor () const;

private:
	Vec3			position;
	Color			color;
};

#endif
