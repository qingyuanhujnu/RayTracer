#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "common.hpp"
#include "model.hpp"
#include "triangle.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "ray.hpp"

class Intersection
{
public:
	struct ShapeIntersection
	{
		enum Facing
		{
			Front,
			Back,
			Unknown
		};

		ShapeIntersection ();

		Vec3 position;
		Vec2 texCoord;
		double distance;
		Facing facing;
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

	struct GeometryIntersection : public MeshIntersection
	{
		GeometryIntersection ();

		UIndex mesh;
	};

	struct ModelIntersection 
	{
		enum IntersectionType {
			Light,
			Geometry,
			Nothing
		};

		ModelIntersection ();

		IntersectionType iSectType;
		GeometryIntersection geometryIntersection;
		LightIntersection lightIntersection;
	};

	enum FacingMode
	{
		OnlyFrontFacing,
		OnlyBackFacing,
		BothFacing
	};

	static bool		RaySphere (const Ray& ray, const Sphere& sphere, ShapeIntersection* intersection);
	static bool		RayBox (const Ray& ray, const Box& box, ShapeIntersection* intersection);
	static bool		RayTriangle (const Ray& ray, const Triangle& triangle, FacingMode facing, ShapeIntersection* intersection);

	static bool		RayMesh (const Ray& ray, const Mesh& mesh, MeshIntersection* intersection);
	static bool		RayGeometry (const Ray& ray, const Model& model, GeometryIntersection* intersection);
	static bool		RayLight (const Ray& ray, const Model& model, LightIntersection* intersection);
	static bool		RayModel (const Ray& ray, const Model& model, ModelIntersection* intersection);
};

#endif
