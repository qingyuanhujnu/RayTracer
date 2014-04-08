#ifndef RAY_HPP
#define RAY_HPP

#include "geometry.hpp"

class Ray
{
public:
	struct Intersection
	{
		Intersection ();
		~Intersection ();

		Coord	position;
		double	distance;
		int		triangle;
	};

	const Coord&	GetDirection () const;
	bool			TriangleIntersection (const Coord& v0, const Coord& v1, const Coord& v2, Coord* intersection) const;
	bool			GeometryIntersection (const Geometry& geometry, Intersection* intersection) const;

protected:
	enum Type
	{
		Undefined	= 0,
		Infinite	= 1,
		Sector		= 2
	};

	Type	type;
	Coord	origin;
	Coord	direction;
	double	length;
};

class SectorRay : public Ray
{
public:
	SectorRay (const Coord& startPoint, const Coord& endPoint);
};

class InfiniteRay : public Ray
{
public:
	InfiniteRay (const Coord& startPoint, const Coord& rayDirection);
};

#endif
