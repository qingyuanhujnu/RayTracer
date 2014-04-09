#ifndef RAY_HPP
#define RAY_HPP

#include "model.hpp"

class Ray
{
public:
	Ray (const Coord& startPoint, const Coord& rayDirection);

	struct TriangleIntersection
	{
		TriangleIntersection ();

		Coord position;
		double distance;
	};

	struct MeshIntersection : public TriangleIntersection
	{
		MeshIntersection ();

		int triangle;
	};

	struct ModelIntersection : public MeshIntersection
	{
		ModelIntersection ();

		int mesh;
	};

	const Coord&	GetDirection () const;
	bool			GetTriangleIntersection (const Coord& v0, const Coord& v1, const Coord& v2, TriangleIntersection* intersection) const;
	bool			GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const;
	bool			GetModelIntersection (const Model& model, ModelIntersection* intersection) const;

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
