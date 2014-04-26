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
		averageColor.Add (RayCast (cameraRay, CameraRay, 0));
	}
	return averageColor.Get ();
}

Color RayTracer::RayCast (const Ray& ray, RayType rayType, int depth) const
{
	if (depth <= 10) {
		Intersection::GeometryIntersection intersection;
		Intersection::FacingMode facingMode = Intersection::OnlyFrontFacing;
		if (rayType != CameraRay) {
			facingMode = Intersection::FrontAndBackFacing;
		}
		if (Intersection::RayGeometry (ray, model, facingMode, &intersection)) {
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
		Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = RayCast (reflectedRay, ReflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	if (material.IsTransparent ()) {
		Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), normal);
		InfiniteRay refractedRay (intersection.position, refractedDirection);
		Color refractedColor = RayCast (refractedRay, RefractedRay, depth + 1);
		color += refractedColor * material.GetTransparency ();
	}

	return Clamp (color);
}

bool RayTracer::IsInShadow (const Vec3& position, const Light& light) const
{
	SectorRay shadowRay (position, light.GetPosition ());
	return Intersection::RayGeometry (shadowRay, model, Intersection::OnlyFrontFacing, NULL);
}
