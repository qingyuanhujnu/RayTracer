#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "coord.hpp"
#include "color.hpp"

class Light
{
public:
	Light ();
	Light (const Coord& position, const Color& color, double ambient, double diffuse, double specular);
	~Light ();

	void			Set (const Coord& position, const Color& color, double ambient, double diffuse, double specular);

	const Coord&	GetPosition () const;

	const Color&	GetColor () const;

	double			GetAmbient () const;
	double			GetDiffuse () const;
	double			GetSpecular () const;

	Color			GetAmbientColor () const;
	Color			GetDiffuseColor () const;
	Color			GetSpecularColor () const;

private:
	Coord			position;

	Color			color;

	double			ambient;
	double			diffuse;
	double			specular;
};

#endif
