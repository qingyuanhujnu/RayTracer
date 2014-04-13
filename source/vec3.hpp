#ifndef Vec3_HPP
#define Vec3_HPP

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

Vec3	Normalize (const Vec3& vec);
double	Distance (const Vec3& avec, const Vec3& bvec);
Vec3	Offset (const Vec3& vec, const Vec3& direction, double distance);

#endif
