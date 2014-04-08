#include "light.hpp"

Light::Light ()
{
}

Light::Light (const Coord& position, const Color& color, double ambient, double diffuse, double specular) :
	position (position),
	color (color),
	ambient (ambient),
	diffuse (diffuse),
	specular (specular)
{
}

Light::~Light ()
{
}

const Coord& Light::GetPosition () const
{
	return position;
}

const Color& Light::GetColor () const
{
	return color;
}

double Light::GetAmbient () const
{
	return ambient;
}

double Light::GetDiffuse () const
{
	return diffuse;
}

double Light::GetSpecular () const
{
	return specular;
}

Color Light::GetAmbientColor () const
{
	return color * ambient;
}

Color Light::GetDiffuseColor () const
{
	return color * diffuse;
}

Color Light::GetSpecularColor () const
{
	return color * specular;
}
