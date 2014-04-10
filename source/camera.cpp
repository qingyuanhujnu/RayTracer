#include "camera.hpp"

Camera::Camera () :
	xFov (0.0),
	yFov (0.0)
{

}

Camera::Camera (const Coord& eye, const Coord& center, const Coord& up, double xFov, double yFov)
{
	Set (eye, center, up, xFov, yFov);
}

Camera::~Camera ()
{

}

void Camera::Set (const Coord& eye, const Coord& center, const Coord& up, double xFov, double yFov)
{
	this->eye = eye;
	this->center = center;
	this->up = up;
	this->xFov = xFov;
	this->yFov = yFov;
}

const Coord& Camera::GetEye () const
{
	return eye;
}

const Coord& Camera::GetCenter () const
{
	return center;
}

const Coord& Camera::GetUp () const
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
