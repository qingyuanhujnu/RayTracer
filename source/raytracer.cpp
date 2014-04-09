#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "shading.hpp"

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

RayTracer::RayTracer (const Geometry& geometry, const Camera& camera, const Light& light) :
	geometry (geometry),
	camera (camera),
	light (light)
{

}

RayTracer::~RayTracer ()
{

}

bool RayTracer::Do (const Parameters& parameters, ResultImage& result)
{
	Image image (camera, parameters.GetResolutionX (), parameters.GetResolutionY (), parameters.GetImageDistance ());
	result.SetResolution (parameters.GetResolutionX (), parameters.GetResolutionY ());

	for (int i = 0; i < parameters.GetResolutionX (); i++) {
		for (int j = 0; j < parameters.GetResolutionY (); j++) {
			InfiniteRay cameraRay (camera.GetEye (), image.GetFieldCenter (i, j) - camera.GetEye ());
			Color fieldColor = ProcessOneRay (cameraRay, 0);
			result.SetColor (i, j, fieldColor);
		}
	}

	return true;
}

static Coord GetReflectedDirection (const Coord& originalDirection, const Coord& normal)
{
	double dotProduct = -(normal * originalDirection);
	return originalDirection + (normal * 2.0 * dotProduct);
}

Color RayTracer::ProcessOneRay (const Ray& ray, int depth)
{
	Ray::Intersection intersection;
	if (ray.GeometryIntersection (geometry, &intersection)) {
		SectorRay shadowRay (intersection.position, light.GetPosition ());
		if (shadowRay.GeometryIntersection (geometry, NULL)) {
			return Color (0.0, 0.0, 0.0);
		}
	} else {
		return Color (0.0, 0.0, 0.0);
	}

	const Triangle& triangle = geometry.GetTriangle (intersection.triangle);
	const Material& material = geometry.GetMaterial (triangle.mat);
	const Coord& normal = geometry.GetNormal (intersection.triangle);
	
	Color currentColor = GetPhongShading (material, light, intersection.position, normal);
	if (depth > 10) {
		return currentColor;
	}

	if (material.IsReflective ()) {
		Coord reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = ProcessOneRay (reflectedRay, depth + 1);
		double reflection = material.GetReflection ();
		currentColor = (currentColor * (1.0 - reflection)) + (reflectedColor * reflection);
	}

	return currentColor;
}
