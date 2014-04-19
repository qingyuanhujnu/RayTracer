#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "average.hpp"
#include "shading.hpp"
#include <omp.h>


RayTracer::RayTracer (const Model& model, const Camera& camera) :
	Renderer (model, camera)
{
}

bool RayTracer::Render (const Parameters& parameters, ResultImage& result)
{
	if (DBGERROR (model.LightCount () == 0)) {
		return false;
	}

	Image image (camera, parameters.GetResolutionX (), parameters.GetResolutionY (), parameters.GetImageDistance (), 3);
	result.SetResolution (parameters.GetResolutionX (), parameters.GetResolutionY ());
	
	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();

	const int procs = omp_get_num_procs ();		// logical cores
#pragma omp parallel for schedule(dynamic, 4) num_threads (procs)
	for (int pix = 0; pix < (resX * resY); ++pix) {
		int x = pix % resX;
		int y = pix / resX;

		Average<Color> averageColor;
		Image::Field field = image.GetField (x, y);
		for (int i = 0; i < field.SampleCount (); i++) {
			InfiniteRay cameraRay (camera.GetEye (), field.GetSample (i) - camera.GetEye ());
			averageColor.Add (RayCast (cameraRay, 0));
		}
		result.SetColor (x, y, averageColor.Get ());
	}

	return true;
}

Color RayTracer::RayCast (const Ray& ray, int depth) const
{
	if (depth <= 10) {
		Ray::GeometryIntersection intersection;
		if (ray.GetGeometryIntersection (model, &intersection)) {
			return RayTrace (ray, intersection, depth);
		}
	}

	Color black (0.0, 0.0, 0.0);
	return black;
}

Color RayTracer::RayTrace (const Ray& ray, const Ray::GeometryIntersection& intersection, int depth) const
{
	const Mesh& mesh = model.GetMesh (intersection.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	const Vec3& normal = mesh.GetNormal (intersection.triangle, intersection.position);

	Color color;
	const Light& light = model.GetLight (0);
	if (!IsInShadow (intersection.position, light)) {
		color += GetPhongShading (material, light, intersection.position, normal, ray);
	} else {
		color = material.GetAmbientColor ();
	}

	if (material.IsReflective ()) {
		Vec3 reflectedDirection = Reflect (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = RayCast (reflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	return Clamp (color);
}

bool RayTracer::IsInShadow (const Vec3& position, const Light& light) const
{
	SectorRay shadowRay (position, light.GetPosition ());
	return shadowRay.GetGeometryIntersection (model, NULL);
}
