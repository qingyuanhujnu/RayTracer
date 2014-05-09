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
	int sampleRes = 3;
	for (int i = 0; i < sampleRes * sampleRes; i++) {
		InfiniteRay cameraRay (camera.GetEye (), field.GetFixSample (sampleRes, i) - camera.GetEye ());
		averageColor.Add (RayCast (cameraRay, 0));
	}
	return averageColor.Get ();
}

Color RayTracer::RayCast (const Ray& ray, int depth) const
{
	if (depth <= 10) {
		Intersection::GeometryIntersection intersection;
		if (Intersection::RayGeometry (ray, model, &intersection)) {
			return RayTrace (ray, intersection, depth);
		}
	}

	Color black (0.0, 0.0, 0.0);
	return black;
}

Color RayTracer::RayTrace (const Ray& ray, const Intersection::GeometryIntersection& intersection, int depth) const
{
	const Mesh& mesh = model.GetMesh (intersection.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
	const Material& material = model.GetMaterial (triangle.material);

	Vec3 normal = mesh.GetNormal (intersection.triangle, intersection.position);
	if (intersection.facing == Intersection::ShapeIntersection::Back) {
		normal = normal * -1.0;
	}

	Color color = material.GetAmbientColor ();
	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		if (!IsInShadow (intersection.position, light)) {
			const Vec3 photonOrigin = light.GetPosition ();
			color += GetPhongShading (material, light, photonOrigin, intersection.position, normal, ray.GetDirection ());
		} 			
	}

	if (material.IsReflective ()) {
		Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = RayCast (reflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	if (material.IsTransparent ()) {
		double transparency = material.GetTransparency ();
		double refractionIndex = material.GetRefractionIndex ();
		color = color * (1.0 - transparency);

		Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), normal, refractionIndex);
		InfiniteRay refractedRay (intersection.position, refractedDirection);
		Color refractedColor = RayCast (refractedRay, depth + 1);
		color += refractedColor * transparency;
	}

	return Clamp (color);
}

bool RayTracer::IsInShadow (const Vec3& position, const Light& light) const
{
	SectorRay shadowRay (position, light.GetPosition ());
	return Intersection::RayGeometry (shadowRay, model, NULL);
}
