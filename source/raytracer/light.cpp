#include "light.hpp"

Light::Light ()
{
}

Light::Light (const Vec3& position, const Color& color)
{
	Set (position, color);
}

Light::~Light ()
{
}

void Light::Set (const Vec3& position, const Color& color)
{
	this->position = position;
	this->color = color;
}

const Vec3& Light::GetPosition () const
{
	return position;
}

const Color& Light::GetColor () const
{
	return color;
}
