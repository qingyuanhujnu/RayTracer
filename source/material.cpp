#include "material.hpp"
#include "common.hpp"

Material::Material ()
{
}

Material::Material (const Color& color, double ambient, double diffuse, double specular, double reflection) :
	color (color),
	ambient (ambient),
	diffuse (diffuse),
	specular (specular),
	reflection (reflection)
{
}

Material::~Material ()
{
}

const Color& Material::GetColor () const
{
	return color;
}

double Material::GetAmbient () const
{
	return ambient;
}

double Material::GetDiffuse () const
{
	return diffuse;
}

double Material::GetSpecular () const
{
	return specular;
}

bool Material::IsReflective () const
{
	return IsPositive (reflection);
}

double Material::GetReflection () const
{
	return reflection;
}

Color Material::GetAmbientColor () const
{
	return color * ambient;
}

Color Material::GetDiffuseColor () const
{
	return color * diffuse;
}

Color Material::GetSpecularColor () const
{
	return color * specular;
}
