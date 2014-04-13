#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "shading.hpp"
#include <omp.h>

RayTracer::Parameters::Parameters (int resolutionX, int resolutionY, double imageDistance) :
	resolutionX (resolutionX),
	resolutionY (resolutionY),
	imageDistance (imageDistance)
{

}

RayTracer::Parameters::~Parameters ()
{

}

int RayTracer::Parameters::GetResolutionX () const
{
	return resolutionX;
}

int RayTracer::Parameters::GetResolutionY () const
{
	return resolutionY;
}

double RayTracer::Parameters::GetImageDistance () const
{
	return imageDistance;
}

RayTracer::ResultImage::ResultImage () :
	resolutionX (0),
	resolutionY (0)
{

}

RayTracer::ResultImage::~ResultImage ()
{

}

void RayTracer::ResultImage::SetResolution (int inResolutionX, int inResolutionY)
{
	resolutionX = inResolutionX;
	resolutionY = inResolutionY;
	image.clear ();
	for (int i = 0; i < resolutionX * resolutionY; i++) {
		image.push_back (Color ());
	}
}

void RayTracer::ResultImage::SetColor (int x, int y, const Color& color)
{
	image[y * resolutionX + x] = color;
}

int RayTracer::ResultImage::GetResolutionX () const
{
	return resolutionX;
}

int RayTracer::ResultImage::GetResolutionY () const
{
	return resolutionY;
}

const Color& RayTracer::ResultImage::GetColor (int x, int y) const
{
	return image[y * resolutionX + x];
}

RayTracer::RayTracer (const Model& model, const Camera& camera, const Light& light) :
	model (model),
	camera (camera),
	light (light)
{

}

RayTracer::~RayTracer ()
{

}

bool RayTracer::Do (const Parameters& parameters, ResultImage& result)
{
	Image image (camera, parameters.GetResolutionX (), parameters.GetResolutionY (), parameters.GetImageDistance (), 3);
	result.SetResolution (parameters.GetResolutionX (), parameters.GetResolutionY ());
	
	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();

	const int procs = omp_get_num_procs ();		// logical cores
#pragma omp parallel for schedule(dynamic, 4) num_threads (procs)
	for (int pix = 0; pix < (resX * resY); ++pix) {
		int x = pix % resX;
		int y = pix / resY;

		Color averageColor (0.0, 0.0, 0.0);
		Image::Field field = image.GetField (x, y);
		for (int i = 0; i < field.SampleCount (); i++) {
			InfiniteRay cameraRay (camera.GetEye (), field.GetSample (i) - camera.GetEye ());
			averageColor = averageColor + RayCast (cameraRay, 0);
		}
		averageColor = averageColor / (double) field.SampleCount ();
		result.SetColor (x, y, averageColor);
	}

	return true;
}

Color RayTracer::RayCast (const Ray& ray, int depth) const
{
	if (depth <= 10) {
		Ray::ModelIntersection intersection;
		if (ray.GetModelIntersection (model, &intersection)) {
			return RayTrace (ray, intersection, depth);
		}
	}

	Color black (0.0, 0.0, 0.0);
	return black;
}

Color RayTracer::RayTrace (const Ray& ray, const Ray::ModelIntersection& intersection, int depth) const
{
	const Mesh& mesh = model.GetMesh (intersection.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	const Vec3& normal = mesh.GetNormal (intersection.triangle, intersection.position);

	Color color;
	if (!IsInShadow (intersection.position)) {
		color += GetPhongShading (material, light, intersection.position, normal);
	} else {
		color = material.GetAmbientColor ();
	}

	if (material.IsReflective ()) {
		Vec3 reflectedDirection = ray.GetReflectedDirection (normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = RayCast (reflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	return Clamp (color);
}

bool RayTracer::IsInShadow (const Vec3& position) const
{
	SectorRay shadowRay (position, light.GetPosition ());
	return shadowRay.GetModelIntersection (model, NULL);
}
