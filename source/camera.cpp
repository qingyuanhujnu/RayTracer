#include "camera.hpp"

Camera::Camera () :
	xFov (0.0),
	yFov (0.0)
{

}

Camera::Camera (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov)
{
	Set (eye, center, up, xFov, yFov);
}

Camera::~Camera ()
{

}

void Camera::Set (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov)
{
	this->eye = eye;
	this->center = center;
	this->up = up;
	this->xFov = xFov;
	this->yFov = yFov;
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
