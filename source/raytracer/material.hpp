#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"
#include "texture.hpp"
#include "vec2.hpp"
#include <memory>
#include <fstream>

class Material
{
	std::shared_ptr<Texture> texture;

	double	ambient;
	double	diffuse;
	double	specular;
	double  shininess;

	double	reflection;
	double	transparency;
	double	refractionIndex;

public:
	Material ();
	Material (std::shared_ptr<Texture> texture, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex);
	~Material ();

	void			Set (std::shared_ptr<Texture> texture, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex);

	double			GetAmbient () const;
	double			GetDiffuse () const;
	double			GetSpecular () const;
	double			GetShininess () const;

	bool			IsReflective () const;
	double			GetReflection () const;

	bool			IsTransparent () const;
	double			GetTransparency () const;
	double			GetRefractionIndex () const;

	Color			GetAmbientColor (const Vec2& texCoord) const;
	Color			GetDiffuseColor (const Vec2& texCoord) const;
	Color			GetSpecularColor (const Vec2& texCoord) const;

private:
	Color			GetColor (const Vec2& texCoord) const;
};

#endif
