#include "light.hpp"
#include "common.hpp"

Light::Light ()
{
}

Light::Light (const Sphere& sphere, const Color& color, const Vec3& attenuation)
{
	Set (sphere, color, attenuation);
}

Light::~Light ()
{
}

void Light::Set (const Sphere& sphere, const Color& color, const Vec3& attenuation)
{
	this->sphere = sphere;
	this->color = color;
	this->attenuation = attenuation;
}

const Vec3& Light::GetPosition () const
{
	return sphere.origin;
}

double Light::GetRadius () const
{
	return sphere.radius;
}

const Color& Light::GetColor () const
{
	return color;
}

const Vec3& Light::GetAttenuation () const
{
	return attenuation;
}

Vec3 Light::GetRandomPoint () const
{
	return sphere.GetRandomPointInVolume ();
}

double Light::GetIntensity (double distance) const
{
	double denom = (attenuation.x + distance * attenuation.y + distance * attenuation.z * attenuation.z);
	if (IsZero (denom)) {
		return 1.0;
	}
	return 1.0 / denom;
}
