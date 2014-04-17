#ifndef RAY_HPP
#define RAY_HPP

#include "common.hpp"
#include "model.hpp"

class Ray
{
public:
	Ray (const Vec3& startPoint, const Vec3& rayDirection);

	struct ShapeIntersection
	{
		ShapeIntersection ();

		Vec3 position;
		double distance;
	};

	struct MeshIntersection : public ShapeIntersection
	{
		MeshIntersection ();

		UIndex triangle;
	};

	struct LightIntersection : public ShapeIntersection
	{
		LightIntersection ();

		UIndex light;
	};

	struct ObjectIntersection : public MeshIntersection
	{
		ObjectIntersection ();

		UIndex mesh;
	};

	struct ModelIntersection 
	{
		ModelIntersection ();

		enum IntersectionType {
			Light,
			Object,
			Nothing
		};

		IntersectionType iSectType;
		ObjectIntersection objectIntersection;
		LightIntersection lightIntersection;
	};

	const Vec3&		GetDirection () const;
	Vec3			GetReflectedDirection (const Vec3& normal) const;

	bool			GetSphereIntersection (const Sphere& sphere, ShapeIntersection* intersection) const;
	bool			GetTriangleIntersection (const Vec3& v0, const Vec3& v1, const Vec3& v2, ShapeIntersection* intersection) const;
	bool			GetMeshIntersection (const Mesh& mesh, MeshIntersection* intersection) const;
	bool			GetObjectIntersection (const Model& model, ObjectIntersection* intersection) const;
	bool			GetLightIntersection (const Model& model, LightIntersection* intersection) const;
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
