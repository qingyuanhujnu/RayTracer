#ifndef RAY_HPP
#define RAY_HPP

#include "common.hpp"
#include "model.hpp"

class Ray
{
public:
	Ray (const Vec3& startPoint, const Vec3& rayDirection);

	struct TriangleIntersection
	{
		TriangleIntersection ();

		Vec3 position;
		double distance;
	};

	struct MeshIntersection : public TriangleIntersection
	{
		MeshIntersection ();

		UIndex triangle;
	};

	struct ModelIntersection : public MeshIntersection
	{
		ModelIntersection ();

		UIndex mesh;
	};

	const Vec3&		GetDirection () const;
	const Vec3&		GetOrigin () const { return origin; }

	bool			GetTriangleIntersection (const Vec3& v0, const Vec3& v1, const Vec3& v2, TriangleIntersection* intersection) const;
	bool			GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const;
	bool			GetModelIntersection (const Model& model, ModelIntersection* intersection) const;

protected:
	virtual bool	IsLengthReached (double currentLength) const = 0;

	Vec3			origin;
	Vec3			direction;
};

class SectorRay : public Ray
{
public:
	SectorRay (const Vec3& startPoint, const Vec3& endPoint);

private:
	virtual bool	IsLengthReached (double currentLength) const override;

	double	length;
};

class InfiniteRay : public Ray
{
public:
	InfiniteRay (const Vec3& startPoint, const Vec3& rayDirection);

private:
	virtual bool	IsLengthReached (double currentLength) const override;
};

#endif
