#include "material.hpp"
#include "common.hpp"

Material::Material () :
	ambient (0.0),
	diffuse (0.0),
	specular (0.0),
	shininess (0.0),
	reflection (0.0),
	transparency (0.0)
{
}

Material::Material (const Color& color, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency)
{
	Set (color, ambient, diffuse, specular, shininess, reflection, transparency);
}

Material::~Material ()
{
}

void Material::Set (const Color& color, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency)
{
	this->color = color;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->reflection = reflection;
	this->transparency = transparency;
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

double Material::GetShininess () const
{
	return shininess;
}

bool Material::IsReflective () const
{
	return IsPositive (reflection);
}

double Material::GetReflection () const
{
	return reflection;
}

bool Material::IsTransparent () const
{
	return IsPositive (transparency);
}

double Material::GetTransparency () const
{
	return transparency;
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
