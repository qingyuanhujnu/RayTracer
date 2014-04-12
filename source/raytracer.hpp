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

private:
	Color		Trace (const Ray& ray, int depth) const;
	Color		Shade (const Ray& ray, const Ray::ModelIntersection& intersection, int depth) const;

	Model		model;
	Camera		camera;
	Light		light;
};

#endif
