#ifndef PATHTRACER_HPP
#define PATHTRACER_HPP

#include "renderer.hpp"
#include "ray.hpp"

class PathTracer : public Renderer 
{
public:
	PathTracer (const Model& model, const Camera& camera, int sampleNum);

private:
	virtual Color	GetFieldColor (const Image::Field& field) override;

	Color			Radiance (const Ray& ray, int depth) const;
	Color			RayCastTowardsLights (const Vec3& position, const Vec3& normal, const Material& material) const;

	int				sampleNum;
};

#endif