#include "Vec3.hpp"
#include "common.hpp"

#include <math.h>

Vec3::Vec3 () :
	x (0.0),
	y (0.0),
	z (0.0)
{
}

Vec3::Vec3 (double x, double y, double z) :
	x (x),
	y (y),
	z (z)
{

}

Vec3::~Vec3 ()
{

}

bool Vec3::operator== (const Vec3& vec) const
{
	return IsEqual (x, vec.x) && IsEqual (y, vec.y) && IsEqual (z, vec.z);
}

Vec3 Vec3::operator+ (const Vec3& vec) const
{
	Vec3 result;
	result.x = x + vec.x;
	result.y = y + vec.y;
	result.z = z + vec.z;
	return result;
}

Vec3 Vec3::operator- (const Vec3& vec) const
{
	Vec3 result;
	result.x = x - vec.x;
	result.y = y - vec.y;
	result.z = z - vec.z;
	return result;
}

Vec3 Vec3::operator^ (const Vec3& vec) const
{
	Vec3 result;
	result.x = y * vec.z - z * vec.y;
	result.y = z * vec.x - x * vec.z;
	result.z = x * vec.y - y * vec.x;
	return result;
}

double Vec3::operator* (const Vec3& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

Vec3 Vec3::operator* (double scalar) const
{
	Vec3 result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

Vec3 Vec3::operator/ (double scalar) const
{
	Vec3 result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	return result;
}

Vec3 Normalize (const Vec3& vec)
{
	double length = sqrt (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (IsZero (length)) {
		return vec;
	}

	return vec * (1.0 / length);
}

Vec3 Offset (const Vec3& vec, const Vec3& direction, double distance)
{
	Vec3 normal = Normalize (direction);
	Vec3 result;
	result.x = vec.x + normal.x * distance;
	result.y = vec.y + normal.y * distance;
	result.z = vec.z + normal.z * distance;
	return result;
}

double Distance (const Vec3& avec, const Vec3& bvec)
{
	double x1 = avec.x;
	double y1 = avec.y;
	double z1 = avec.z;
	double x2 = bvec.x;
	double y2 = bvec.y;
	double z2 = bvec.z;

	return sqrt ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}