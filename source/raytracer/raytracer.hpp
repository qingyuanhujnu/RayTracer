#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "common.hpp"
#include "renderer.hpp"
#include "ray.hpp"

class RayTracer : public Renderer
{
public:
	RayTracer (const Model& model, const Camera& camera, const Light& light);
	
	void		Render (const Parameters& parameters, ResultImage& result) override;

private:
	Color		RayCast (const Ray& ray, int depth) const;
	Color		RayTrace (const Ray& ray, const Ray::ModelIntersection& intersection, int depth) const;
	
	bool		IsInShadow (const Vec3& position) const;
};

#endif
