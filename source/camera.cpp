#include "camera.hpp"

Camera::Camera ()
{

}

Camera::Camera (const Coord& eye, const Coord& center, const Coord& up, double xFov, double yFov) :
	eye (eye),
	center (center),
	up (up),
	xFov (xFov),
	yFov (yFov)
{

}

Camera::~Camera ()
{

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
