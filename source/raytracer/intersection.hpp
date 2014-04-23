#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "common.hpp"
#include "model.hpp"
#include "box.hpp"
#include "sphere.hpp"
#include "ray.hpp"

class Intersection
{
public:
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

	struct GeometryIntersection : public MeshIntersection
	{
		GeometryIntersection ();

		UIndex mesh;
	};

	struct ModelIntersection 
	{
		ModelIntersection ();

		enum IntersectionType {
			Light,
			Geometry,
			Nothing
		};

		IntersectionType iSectType;
		GeometryIntersection geometryIntersection;
		LightIntersection lightIntersection;
	};

	static bool		RaySphere (const Ray& ray, const Sphere& sphere, ShapeIntersection* intersection);
	static bool		RayBox (const Ray& ray, const Box& box, ShapeIntersection* intersection);
	static bool		RayTriangle (const Ray& ray, const Vec3& v0, const Vec3& v1, const Vec3& v2, ShapeIntersection* intersection);

	static bool		RayMesh (const Ray& ray, const Mesh& mesh, MeshIntersection* intersection);
	static bool		RayGeometry (const Ray& ray, const Model& model, GeometryIntersection* intersection);
	static bool		RayLight (const Ray& ray, const Model& model, LightIntersection* intersection);
	static bool		RayModel (const Ray& ray, const Model& model, ModelIntersection* intersection);
};

#endif
