#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "coord.hpp"

class Camera
{
public:
	Camera ();
	Camera (const Coord& eye, const Coord& center, const Coord& up, double xFov, double yFov);
	~Camera ();

	const Coord&	GetEye () const;
	const Coord&	GetCenter () const;
	const Coord&	GetUp () const;

	double			GetXFov () const;
	double			GetYFov () const;

private:
	Coord	eye;
	Coord	center;
	Coord	up;
	double	xFov;
	double	yFov;
};

#endif
