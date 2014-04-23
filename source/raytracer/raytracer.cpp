#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "average.hpp"
#include "shading.hpp"

RayTracer::RayTracer (const Model& model, const Camera& camera) :
	Renderer (model, camera)
{
}

Color RayTracer::GetFieldColor (const Image::Field& field)
{
	Average<Color> averageColor;
	for (int i = 0; i < field.SampleCount (); i++) {
		InfiniteRay cameraRay (camera.GetEye (), field.GetSample (i) - camera.GetEye ());
		averageColor.Add (RayCast (cameraRay, 0));
	}
	return averageColor.Get ();
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
	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		if (!IsInShadow (intersection.position, light)) {
			color += GetPhongShading (material, light, intersection.position, normal, ray);
		} else {
			color += material.GetAmbientColor ();
		}
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
