#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "ray.hpp"

class RayTracer
{
public:
	class Parameters
	{
	public:
		Parameters (int resolutionX, int resolutionY, double imageDistance);
		~Parameters ();

		int		GetResolutionX () const;
		int		GetResolutionY () const;
		double	GetImageDistance () const;
	
	private:
		int		resolutionX;
		int		resolutionY;
		double	imageDistance;
	};

	class ResultImage
	{
	public:
		ResultImage ();
		~ResultImage ();

		void				SetResolution (int resolutionX, int resolutionY);
		void				SetColor (int x, int y, const Color& color);

		int					GetResolutionX () const;
		int					GetResolutionY () const;
		const Color&		GetColor (int x, int y) const;

	private:
		int					resolutionX;
		int					resolutionY;
		std::vector<Color>	image;
	};

	RayTracer (const Model& model, const Camera& camera, const Light& light);
	~RayTracer ();

	bool		Do (const Parameters& parameters, ResultImage& result);

protected:
	Color		RayTrace (const Ray& ray, const Ray::ModelIntersection& intersection, int depth) const;
	Color		RayCast (const Ray& ray, int depth) const;
	
	bool		IsInShadow (const Vec3& position) const;

	Model		model;
	Camera		camera;
	Light		light;
};


class PathTracer : public RayTracer {
public:
	enum IntersectionType {
		XNothing,
		XModel,
		XLight
	};

	PathTracer (const Model& model, const Camera& camera, const Light& light) :
		RayTracer (model, camera, light)
	{}

	bool				Do (const Parameters& parameters, ResultImage& result);
	Color				Radiance (const Ray& ray, int depth) const;
	IntersectionType	RayCast (const Ray& ray, Vec3& isect) const;
};

#endif
