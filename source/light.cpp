#include "light.hpp"

Light::Light () :
	ambient (0.0),
	diffuse (0.0),
	specular (0.0)
{
}

Light::Light (const Vec3& position, const Color& color, double ambient, double diffuse, double specular)
{
	Set (position, color, ambient, diffuse, specular);
}

Light::~Light ()
{
}

void Light::Set (const Vec3& position, const Color& color, double ambient, double diffuse, double specular)
{
	this->position = position;
	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

const Vec3& Light::GetPosition () const
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
