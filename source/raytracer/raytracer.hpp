#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "common.hpp"
#include "renderer.hpp"
#include "ray.hpp"

class RayTracer : public Renderer
{
public:
	RayTracer (const Model& model, const Camera& camera);
	
private:
	virtual Color	GetFieldColor (const Image::Field& field) override;
	Color			RayCast (const Ray& ray, int depth) const;
	Color			RayTrace (const Ray& ray, const Ray::GeometryIntersection& intersection, int depth) const;
	
	bool			IsInShadow (const Vec3& position, const Light& light) const;
};

#endif
