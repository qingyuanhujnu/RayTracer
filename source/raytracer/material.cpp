#include "material.hpp"
#include "common.hpp"

Material::Material () :
	texture (nullptr),
	ambient (0.0),
	diffuse (0.0),
	specular (0.0),
	shininess (0.0),
	reflection (0.0),
	transparency (0.0),
	refractionIndex (0.0)
{
}

Material::Material (std::shared_ptr<Texture> texture, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex)
{
	Set (texture, ambient, diffuse, specular, shininess, reflection, transparency, refractionIndex);
}

Material::~Material ()
{
}

void Material::Set (std::shared_ptr<Texture> texture, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex)
{
	this->texture = texture;
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
	this->reflection = reflection;
	this->transparency = transparency;
	this->refractionIndex = refractionIndex;
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

double Material::GetRefractionIndex () const
{
	return refractionIndex;
}

Color Material::GetAmbientColor (const Vec2& texCoord) const
{
	return GetColor (texCoord) * ambient;
}

Color Material::GetDiffuseColor (const Vec2& texCoord) const
{
	return GetColor (texCoord) * diffuse;
}

Color Material::GetSpecularColor (const Vec2& texCoord) const
{
	return GetColor (texCoord) * specular;
}

Color Material::GetColor (const Vec2& texCoord) const
{
	return texture->Sample (texCoord);
}