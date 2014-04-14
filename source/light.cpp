#include "light.hpp"

Light::Light () :
	r (1.5),
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

bool Light::Intersect (const Ray& ray, Vec3& isect) const
{
	Vec3 op = position - ray.GetOrigin (); // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 

	double b = op * ray.GetDirection ();
	double det = b*b - op*op + r*r;		// (b^2 - 4ac)/4
	if (det < 0)
		return false;
	else
		det = sqrt (det);
	
	double t = 0;
	if (b - det > EPS)
		t = b - det;
	else if (b + det > EPS)
		t = b + det;
	else
		return false;
	
	isect = ray.GetOrigin () + ray.GetDirection () * t;
	return true;
}
