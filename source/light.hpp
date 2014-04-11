#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Vec3.hpp"
#include "color.hpp"

class Light
{
public:
	Light ();
	Light (const Vec3& position, const Color& color, double ambient, double diffuse, double specular);
	~Light ();

	void			Set (const Vec3& position, const Color& color, double ambient, double diffuse, double specular);

	const Vec3&	GetPosition () const;

	const Color&	GetColor () const;

	double			GetAmbient () const;
	double			GetDiffuse () const;
	double			GetSpecular () const;

	Color			GetAmbientColor () const;
	Color			GetDiffuseColor () const;
	Color			GetSpecularColor () const;

private:
	Vec3			position;

	Color			color;

	double			ambient;
	double			diffuse;
	double			specular;
};

#endif
