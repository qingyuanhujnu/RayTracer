#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "vec3.hpp"
#include "color.hpp"
#include "ray.hpp"

class Light
{
public:
	Light ();
	Light (const Vec3& position, const Color& color, double ambient, double diffuse, double specular);
	~Light ();

	void			Set (const Vec3& position, const Color& color, double ambient, double diffuse, double specular);

	const Vec3&		GetPosition () const;
	double			GetR () const { return r; }

	const Color&	GetColor () const;

	double			GetAmbient () const;
	double			GetDiffuse () const;
	double			GetSpecular () const;

	Color			GetAmbientColor () const;
	Color			GetDiffuseColor () const;
	Color			GetSpecularColor () const;

	bool			Intersect (const Ray& ray, Vec3& isect) const;

private:
	Vec3			position;
	double			r;

	Color			color;

	double			ambient;
	double			diffuse;
	double			specular;
};

#endif
