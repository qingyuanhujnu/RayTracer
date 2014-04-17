#ifndef PATHTRACER_HPP
#define PATHTRACER_HPP

#include "renderer.hpp"
#include "ray.hpp"

class PathTracer : public Renderer 
{
public:
	PathTracer (const Model& model, const Camera& camera);

	bool		Render (const Parameters& parameters, ResultImage& result) override;

private:
	enum IntersectionType {
		XNothing,
		XModel,
		XLight
	};

	Color				Radiance (const Ray& ray, int depth) const;
	IntersectionType	RayCast (const Ray& ray, Vec3& isect) const;
};

#endif