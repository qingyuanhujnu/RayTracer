#ifndef RAY_HPP
#define RAY_HPP

#include "geometry.hpp"

class Ray
{
public:
	Ray (const Coord& startPoint, const Coord& rayDirection);

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
	virtual bool	IsLengthReached (double currentLength) const = 0;

	Coord			origin;
	Coord			direction;
};

class SectorRay : public Ray
{
public:
	SectorRay (const Coord& startPoint, const Coord& endPoint);

private:
	virtual bool	IsLengthReached (double currentLength) const override;

	double	length;
};

class InfiniteRay : public Ray
{
public:
	InfiniteRay (const Coord& startPoint, const Coord& rayDirection);

private:
	virtual bool	IsLengthReached (double currentLength) const override;
};

#endif
