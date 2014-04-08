#include "coord.hpp"
#include "common.hpp"

#include <math.h>

Coord::Coord () :
	x (0.0),
	y (0.0),
	z (0.0)
{
}

Coord::Coord (double x, double y, double z) :
	x (x),
	y (y),
	z (z)
{

}

Coord::~Coord ()
{

}

Coord Coord::operator+ (const Coord& coord) const
{
	Coord result;
	result.x = x + coord.x;
	result.y = y + coord.y;
	result.z = z + coord.z;
	return result;
}

Coord Coord::operator- (const Coord& coord) const
{
	Coord result;
	result.x = x - coord.x;
	result.y = y - coord.y;
	result.z = z - coord.z;
	return result;
}

Coord Coord::operator^ (const Coord& coord) const
{
	Coord result;
	result.x = y * coord.z - z * coord.y;
	result.y = z * coord.x - x * coord.z;
	result.z = x * coord.y - y * coord.x;
	return result;
}

double Coord::operator* (const Coord& coord) const
{
	return x * coord.x + y * coord.y + z * coord.z;
}

Coord Coord::operator* (double scalar) const
{
	Coord result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

Coord Normalize (const Coord& coord)
{
	double length = sqrt (coord.x * coord.x + coord.y * coord.y + coord.z * coord.z);
	if (IsZero (length)) {
		return coord;
	}

	return coord * (1.0 / length);
}

Coord Offset (const Coord& coord, const Coord& direction, double distance)
{
	Coord normal = Normalize (direction);
	Coord result;
	result.x = coord.x + normal.x * distance;
	result.y = coord.y + normal.y * distance;
	result.z = coord.z + normal.z * distance;
	return result;
}

double Distance (const Coord& aCoord, const Coord& bCoord)
{
	double x1 = aCoord.x;
	double y1 = aCoord.y;
	double z1 = aCoord.z;
	double x2 = bCoord.x;
	double y2 = bCoord.y;
	double z2 = bCoord.z;

	return sqrt ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}