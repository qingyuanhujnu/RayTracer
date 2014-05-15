#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vec3.hpp"
#include "color.hpp"
#include "sphere.hpp"
#include "box.hpp"

class Light
{
public:
	enum Shape
	{
		SphereShape,
		BoxShape
	};

	Light ();
	Light (const Sphere& sphere, const Color& color, const Vec3& attenuation);
	Light (const Box& box, const Color& color, const Vec3& attenuation);
	~Light ();

	void			Set (const Sphere& sphere, const Color& color, const Vec3& attenuation);
	void			Set (const Box& box, const Color& color, const Vec3& attenuation);

	Shape			GetShape () const;
	const Vec3&		GetPosition () const;

	const Sphere&	GetSphere () const;
	const Box&		GetBox () const;

	const Color&	GetColor () const;
	const Vec3&		GetAttenuation () const;
	
	Vec3			GetRandomPoint () const;
	double			GetIntensity (double distance) const;

private:
	Shape			shape;
	Sphere			sphere;
	Box				box;

	Color			color;
	Vec3			attenuation;
};

#endif
