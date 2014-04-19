#ifndef VEC3_HPP
#define VEC3_HPP

class Vec3
{
public:
	Vec3 ();
	Vec3 (double x, double y, double z);
	~Vec3 ();

	bool	operator== (const Vec3& vec) const;

	Vec3	operator+ (const Vec3& vec) const;
	Vec3	operator- (const Vec3& vec) const;
	Vec3	operator^ (const Vec3& vec) const;
	double	operator* (const Vec3& vec) const;
	Vec3	operator* (double scalar) const;
	Vec3	operator/ (double scalar) const;

	double	x;
	double	y;
	double	z;
};

Vec3	operator* (double scalar, const Vec3 vec);
Vec3	operator/ (double scalar, const Vec3 vec);

Vec3	Normalize (const Vec3& vec);
double	Distance (const Vec3& aVec, const Vec3& bVec);
Vec3	Offset (const Vec3& vec, const Vec3& direction, double distance);
Vec3	Reflect (const Vec3& vec, const Vec3& normal);

#endif
