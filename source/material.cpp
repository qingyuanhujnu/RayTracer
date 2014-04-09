#include "material.hpp"
#include "common.hpp"

Material::Material ()
{
}

Material::Material (const Color& color, double ambient, double diffuse, double specular, double reflection)
{
	Set (color, ambient, diffuse, specular, reflection);
}

Material::~Material ()
{
}

void Material::Set (const Color& color, double ambient, double diffuse, double specular, double reflection)
{
	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->reflection = reflection;
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
