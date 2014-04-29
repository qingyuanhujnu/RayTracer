#ifndef PATHTRACER2_HPP
#define PATHTRACER2_HPP

#include "renderer.hpp"
#include "intersection.hpp"
#include "ray.hpp"

class PathTracer2 : public Renderer 
{
public:
	PathTracer2 (const Model& model, const Camera& camera);

private:
	virtual Color	GetFieldColor (const Image::Field& field) override;

	Color			Trace (const Ray& ray, int depth);
	Color			SampleLights (const Material& material, const Vec3& point, const Vec3& normal, const Vec3& viewDirection);
	Color			SampleGeometry (const Vec3& point, const Vec3& normal, int depth);
};

#endif