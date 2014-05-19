#include "camera.hpp"
#include "common.hpp"
#include "random.hpp"

Camera::Camera () :
	xFov (0.0),
	yFov (0.0)
{

}

Camera::Camera (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov, double focalLength, double apertureRadius)
{
	Set (eye, center, up, xFov, yFov, focalLength, apertureRadius);
}

Camera::~Camera ()
{

}

void Camera::Set (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov, double focalLength, double apertureRadius)
{
	this->eye = eye;
	this->center = center;
	this->up = up;
	this->xFov = xFov;
	this->yFov = yFov;
	this->focalLength = focalLength;
	this->apertureRadius = apertureRadius;
}

const Vec3& Camera::GetEye () const
{
	return eye;
}

const Vec3& Camera::GetCenter () const
{
	return center;
}

const Vec3& Camera::GetUp () const
{
	return up;
}

double Camera::GetXFov () const
{
	return xFov;
}

double Camera::GetYFov () const
{
	return yFov;
}

double Camera::GetFocalLength () const
{
	return focalLength;
}

double Camera::GetApertureRadius () const
{
	return apertureRadius;
}

Vec3 Camera::GetFocalPoint (const Vec3& targetPoint) const
{
	Vec3 focalPoint = Offset (eye, targetPoint - eye, focalLength);
	return focalPoint;
}

Vec3 Camera::GetRandomAperturePoint (const Vec3& xDir, const Vec3& yDir) const
{
	double randRadius = RandomInRange (0.0, apertureRadius);
	double randAngle = RandomInRange (0.0, 2.0 * PI);
	Vec3 apertureOffset = PolarToCartesian (randRadius, randAngle);
	
	Vec3 result = eye;
	result = Offset (result, xDir, apertureOffset.x);
	result = Offset (result, yDir, apertureOffset.y);
	return result;
}
