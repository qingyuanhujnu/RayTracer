#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vec3.hpp"

class Camera
{
public:
	Camera ();
	Camera (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov, double focalLength, double apertureRadius);
	~Camera ();

	void			Set (const Vec3& eye, const Vec3& center, const Vec3& up, double xFov, double yFov, double focalLength, double apertureRadius);

	const Vec3&		GetEye () const;
	const Vec3&		GetCenter () const;
	const Vec3&		GetUp () const;

	double			GetXFov () const;
	double			GetYFov () const;

	double			GetFocalLength () const;
	double			GetApertureRadius () const;

	Vec3			GetFocalPoint (const Vec3& direction) const;
	Vec3			GetRandomAperturePoint (const Vec3& xDir, const Vec3& yDir) const;

private:
	Vec3			eye;
	Vec3			center;
	Vec3			up;
	
	double			xFov;
	double			yFov;

	double			focalLength;
	double			apertureRadius;
};

#endif
