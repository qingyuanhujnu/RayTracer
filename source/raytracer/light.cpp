#include "light.hpp"
#include "common.hpp"

Light::Light () :
	shape (SphereShape)
{
}

Light::Light (const Sphere& sphere, const Color& color, const Vec3& attenuation)
{
	Set (sphere, color, attenuation);
}

Light::Light (const Box& box, const Color& color, const Vec3& attenuation)
{
	Set (box, color, attenuation);
}

Light::~Light ()
{
}

void Light::Set (const Sphere& sphere, const Color& color, const Vec3& attenuation)
{
	this->shape = SphereShape;
	this->sphere = sphere;
	this->color = color;
	this->attenuation = attenuation;
}

void Light::Set (const Box& box, const Color& color, const Vec3& attenuation)
{
	this->shape = BoxShape;
	this->box = box;
	this->color = color;
	this->attenuation = attenuation;
}

Light::Shape Light::GetShape () const
{
	return shape;
}

const Vec3& Light::GetPosition () const
{
	if (shape == SphereShape) {
		return sphere.origin;
	} else if (shape == BoxShape) {
		return box.center;
	}
	return sphere.origin;
}

const Sphere& Light::GetSphere () const
{
	return sphere;
}

const Box& Light::GetBox () const
{
	return box;
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
	if (shape == SphereShape) {
		return sphere.GetRandomPointInVolume ();
	} else if (shape == BoxShape) {
		return box.GetRandomPointInVolume ();
	}
	return Vec3 (0.0, 0.0, 0.0);
}

double Light::GetIntensity (double distance) const
{
	double denom = (attenuation.x + distance * attenuation.y + distance * attenuation.z * attenuation.z);
	if (IsZero (denom)) {
		return 1.0;
	}
	return 1.0 / denom;
}
