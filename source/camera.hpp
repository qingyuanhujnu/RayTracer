#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vec3.hpp"

class Camera
{
public:
	Camera ();
	Camera (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov);
	~Camera ();

	void			Set (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov);

	const Vec3&		GetEye () const;
	const Vec3&		GetCenter () const;
	const Vec3&		GetUp () const;

	double			GetXFov () const;
	double			GetYFov () const;

private:
	Vec3	eye;
	Vec3	center;
	Vec3	up;
	
	double	xFov;
	double	yFov;
};

#endif
