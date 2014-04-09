#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "color.hpp"

class Material
{
public:
	Material ();
	Material (const Color& color, double ambient, double diffuse, double specular, double reflection);
	~Material ();

	void			Set (const Color& color, double ambient, double diffuse, double specular, double reflection);

	const Color&	GetColor () const;
	
	double			GetAmbient () const;
	double			GetDiffuse () const;
	double			GetSpecular () const;

	bool			IsReflective () const;
	double			GetReflection () const;

	Color			GetAmbientColor () const;
	Color			GetDiffuseColor () const;
	Color			GetSpecularColor () const;

private:
	Color	color;

	double	ambient;
	double	diffuse;
	double	specular;

	double	reflection;
};

#endif
