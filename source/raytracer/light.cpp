#include "light.hpp"

Light::Light ()
{
}

Light::Light (const Vec3& position, const Color& color, double radius)
{
	Set (position, color, radius);
}

Light::~Light ()
{
}

void Light::Set (const Vec3& position, const Color& color, double radius)
{
	this->position = position;
	this->color = color;
	this->radius = radius;
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
