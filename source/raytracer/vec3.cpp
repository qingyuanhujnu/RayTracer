#include "vec3.hpp"
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

Vec3 Vec3::operator+ (double scalar) const
{
	Vec3 result;
	result.x = x + scalar;
	result.y = y + scalar;
	result.z = z + scalar;
	return result;
}

Vec3 Vec3::operator- (double scalar) const
{
	Vec3 result;
	result.x = x - scalar;
	result.y = y - scalar;
	result.z = z - scalar;
	return result;
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

Vec3 operator* (double scalar, const Vec3& vec)
{
	return vec.operator* (scalar);
}

void	FastVecSub (const Vec3& op1, const Vec3& op2, Vec3& result)
{
	result.x = op1.x - op2.x;
	result.y = op1.y - op2.y;
	result.z = op1.z - op2.z;
}

void	FastVecMult (const Vec3& op1, const Vec3& op2, Vec3& result)
{
	result.x = op1.y * op2.z - op1.z * op2.y;
	result.y = op1.z * op2.x - op1.x * op2.z;
	result.z = op1.x * op2.y - op1.y * op2.x;
}

Vec3 Normalize (const Vec3& vec)
{
	double length = sqrt (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	if (DBGERROR (IsZero (length))) {
		return vec;
	}

	return vec * (1.0 / length);
}

double Distance (const Vec3& aVec, const Vec3& bVec)
{
	double x1 = aVec.x;
	double y1 = aVec.y;
	double z1 = aVec.z;
	double x2 = bVec.x;
	double y2 = bVec.y;
	double z2 = bVec.z;

	return sqrt ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
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

Vec3 GetReflectedDirection (const Vec3& direction, const Vec3& normal)
{
	double dotProduct = normal * direction;
	return direction - (2.0 * normal * dotProduct);
}
