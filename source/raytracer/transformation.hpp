#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include "vec3.hpp"

class Transformation
{
public:
	Transformation ();
	Transformation (const Transformation& source);
	~Transformation ();

	void	SetIdentity ();

	void	SetTranslation (const Vec3& translation);
	void	SetRotation (const Vec3& axis, double angle, const Vec3* origo);
	void	SetRotationX (double angle);
	void	SetRotationY (double angle);
	void	SetRotationZ (double angle);

	void	Append (const Transformation& mat);
	void	AppendTranslation (const Vec3& translation);
	void	AppendRotationX (double angle);
	void	AppendRotationY (double angle);
	void	AppendRotationZ (double angle);
	void	AppendRotationXYZ (const Vec3& rotation);

	Vec3	Apply (const Vec3& vec) const;
	Vec3	ApplyRotation (const Vec3& vec) const;
	void	Clone (Transformation& mat) const;

private:
	double	matrix[16];
};

#endif
