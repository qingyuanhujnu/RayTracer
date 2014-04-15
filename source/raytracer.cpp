#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "shading.hpp"
#include "random.hpp"
#include <omp.h>
#include <limits>
#include <stdio.h>

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

bool PathTracer::Do (const Parameters& parameters, ResultImage& result)
{
	Image image (camera, parameters.GetResolutionX (), parameters.GetResolutionY (), parameters.GetImageDistance (), 1);
	result.SetResolution (parameters.GetResolutionX (), parameters.GetResolutionY ());

	const int sampleNum = 8; // per pixel

	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();

	const int procs = omp_get_num_procs ();		// logical cores
#pragma omp parallel for schedule(dynamic, 4) num_threads (procs - 1)
	for (int pix = 0; pix < (resX * resY); ++pix) {

		float progress = float (pix) / (resX * resY) * 100;
		printf ("%.3f%%\n", progress);

		int x = pix % resX;
		int y = pix / resY;

		Color fieldColor;
		for (int s = 0; s < sampleNum; ++s) {
			InfiniteRay cameraRay (camera.GetEye (), image.GetFieldRandom (x, y) - camera.GetEye ());
			Color color = Clamp (Radiance (cameraRay, 0));
			fieldColor += (color / sampleNum);
		}
		result.SetColor (x, y, fieldColor);
	}

	return true;
}

// based on smallpt
Color PathTracer::Radiance (const Ray& ray, int depth) const
{
	Ray::ModelIntersection isect;
	if (!ray.GetModelIntersection (model, &isect)) {
		return Color (0.0, 0.0, 0.0);
	}

	const Mesh& mesh = model.GetMesh (isect.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (isect.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	const Vec3& normal = mesh.GetNormal (isect.triangle, isect.position);

	Color colDiffuse = material.GetDiffuseColor ();
	double cDiffIntensity = std::fmax (std::fmax (colDiffuse.r, colDiffuse.g), colDiffuse.b);
	
	if (++depth > 5) {
		double p = random ();
		if (cDiffIntensity < p) {				// russian roulette: what a suicidal path we took!
			return Color (0.0, 0.0, 0.0);
		}
	}

	// Diffuse reflection
	// Get random direction in the hemisphere given by hit surface's normal
	Color color;

	double r1 = random () * 2 * PI;		// random angle around
	double r2 = randomNormalDistrib ();		// distance that is weighted towards normals direction
	double r2s = sqrt (r2);

	Vec3 w = normal;
	if (normal * ray.GetDirection () > 0)
		w = w * -1.0;
	Vec3 u = Normalize ((fabs (w.x) > 0.1 ? Vec3 (0, 1, 0) : Vec3 (1, 0, 0)) ^ w);
	Vec3 v = w ^ u;
	Vec3 randomDir = Normalize (u*cos (r1)*r2s + v*sin (r1)*r2s + w*sqrt (1 - r2));		// Random direction in hemisphere

	{
		// Loop over any lights. This code only works for spherical lights right now but it could be modified to work with any bound box. 
		// Note that path tracing works without this but shooting rays towards the light's sphere makes it converge in less samples.
		//for (int i = 0; i<lights; i++){		// right now there is only one light
			// Start a random ray toward light sphere.
			Vec3 sw = light.GetPosition () - isect.position;
			Vec3 su = Normalize ((fabs (sw.x) > .1 ? Vec3 (0, 1, 0) : Vec3 (1, 0, 0)) ^ sw);
			Vec3 sv = sw ^ su;
			double lr = light.GetR ();
			double cos_a_max = sqrt (1 - lr*lr / ((isect.position - light.GetPosition ()) * (isect.position - light.GetPosition ())));		// angle of the cone in which we see the light
			double eps1 = random (), eps2 = random ();
			double cos_a = 1 - eps1 + eps1*cos_a_max;
			double sin_a = sqrt (1 - cos_a*cos_a);
			double phi = 2 * PI * eps2;
			Vec3 l = su*cos (phi)*sin_a + sv*sin (phi)*sin_a + sw*cos_a;
			l = Normalize (l);
			Vec3 lightIsect;
			if (RayCast (InfiniteRay (isect.position, l), lightIsect) == PathTracer::XLight) {		// TODO: when we have more than one light we need to know if we hit the one we intended to hit!
				double omega = 2 * PI * (1 - cos_a_max);
				color += GetPhongShading (material, light, isect.position, normal) * omega * INV_PI;
			}
		//}
	}

	InfiniteRay shadowRay (isect.position, randomDir);
	Vec3 shadowISect;
	PathTracer::IntersectionType shadowISectType = RayCast (shadowRay, shadowISect);

	if (shadowISectType == PathTracer::XLight) {		// light hit!
		color += GetPhongShading (material, light, isect.position, normal);
	} else if (shadowISectType == PathTracer::XModel) {
		color += Radiance (shadowRay, depth) * cDiffIntensity;
	}

	// Ideal reflection
	if (material.IsReflective ()) {
		Vec3 reflectedDirection = ray.GetReflectedDirection (normal);
		InfiniteRay reflectedRay (isect.position, reflectedDirection);
		Color reflectedColor = Radiance (reflectedRay, depth + 1);
		double reflection = material.GetReflection ();
		color += (reflectedColor * reflection);
	}

	return Clamp (color);
}

PathTracer::IntersectionType PathTracer::RayCast (const Ray& ray, Vec3& isect) const
{
	// Is there a light in ray's path?
	Vec3 lightIsect;
	double lightDist = std::numeric_limits<double>::max ();
	if (light.Intersect (ray, lightIsect)) {
		lightDist = Distance (lightIsect, ray.GetOrigin ());
	}

	// Is anything in the model in ray's path?
	Ray::ModelIntersection modelIsect;
	double modelDist = std::numeric_limits<double>::max ();
	if (ray.GetModelIntersection (model, &modelIsect)) {
		modelDist = modelIsect.distance;
	}

	if (lightDist < modelDist) {		// light hit!
		isect = lightIsect;
		return PathTracer::XLight;
	}
	else if (modelDist < lightDist && modelDist < std::numeric_limits<double>::max ()) {
		isect = modelIsect.position;
		return PathTracer::XModel;
	}

	return XNothing;
}
