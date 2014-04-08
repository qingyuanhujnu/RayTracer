#ifndef COORD_HPP
#define COORD_HPP

class Coord
{
public:
	Coord ();
	Coord (double x, double y, double z);
	~Coord ();

	Coord	operator+ (const Coord& coord) const;
	Coord	operator- (const Coord& coord) const;
	Coord	operator^ (const Coord& coord) const;
	double	operator* (const Coord& coord) const;
	Coord	operator* (double scalar) const;

	double x;
	double y;
	double z;
};

Coord	Normalize (const Coord& coord);
double	Distance (const Coord& aCoord, const Coord& bCoord);
Coord	Offset (const Coord& coord, const Coord& direction, double distance);

#endif
