#ifndef PATHTRACER2_HPP
#define PATHTRACER2_HPP

#include "renderer.hpp"
#include "intersection.hpp"
#include "ray.hpp"

class PathTracer2 : public Renderer 
{
public:
	PathTracer2 (const Model& model, const Camera& camera, int sampleNum);

private:
	virtual Color	GetFieldColor (const Image::Field& field) const override;

	Color			Trace (const Ray& ray, int depth) const;
	Color			SampleLights (const Material& material, const Vec3& point, const Vec3& normal, const Vec3& viewDirection) const;
	Color			SampleGeometry (const Vec3& point, const Vec3& normal, int depth) const;
};

#endif
