#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "common.hpp"
#include "renderer.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class RayTracer : public Renderer
{
public:
	RayTracer (const Model& model, const Camera& camera);
	
private:
	enum RayType
	{
		CameraRay,
		ReflectedRay,
		RefractedRay
	};

	virtual Color	GetFieldColor (const Image::Field& field) override;
	Color			RayCast (const Ray& ray, RayType rayType, int depth) const;
	Color			RayTrace (const Ray& ray, const Intersection::GeometryIntersection& intersection, int depth) const;
	
	bool			IsInShadow (const Vec3& position, const Light& light) const;
};

#endif
