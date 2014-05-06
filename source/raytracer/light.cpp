#include "light.hpp"

Light::Light ()
{
}

Light::Light (const Vec3& position, const Color& color, double radius, const Vec3& attenuation)
{
	Set (position, color, radius, attenuation);
}

Light::~Light ()
{
}

void Light::Set (const Vec3& position, const Color& color, double radius, const Vec3& attenuation)
{
	this->position = position;
	this->color = color;
	this->radius = radius;
	this->attenuation = attenuation;
}

const Vec3& Light::GetPosition () const
{
	return position;
}

const Color& Light::GetColor () const
{
	return color;
}

double Light::GetRadius () const
{
	return radius;
}

const Vec3& Light::GetAttenuation () const
{
	return attenuation;
}

double Light::GetIntensity (double distance) const
{
	return 1.0 / (attenuation.x + distance * attenuation.y + distance * attenuation.z * attenuation.z);
}
