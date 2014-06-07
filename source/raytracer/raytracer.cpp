#include "raytracer.hpp"
#include "ray.hpp"
#include "image.hpp"
#include "common.hpp"
#include "random.hpp"
#include "average.hpp"
#include "shading.hpp"

RayTracer::RayTracer (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera, sampleNum)
{
}

Color RayTracer::GetFieldColor (const Image::Field& field) const
{
	Average<Color> averageColor;
	int sampleRes = (int) sqrt ((double) sampleNum);
	for (int i = 0; i < sampleRes * sampleRes; i++) {
		InfiniteRay cameraRay (camera.GetEye (), field.GetFixSample (sampleRes, i) - camera.GetEye ());
		averageColor.Add (Trace (cameraRay, 0));
	}
	return averageColor.Get ();
}

Color RayTracer::Trace (const Ray& ray, int depth) const
{
	Color color;
	if (depth > 10) {
		return color;
	}

	Intersection::ModelIntersection modelIntersection;
	if (!Intersection::RayModel (ray, model, &modelIntersection)) {
		return color;
	}
	
	if (modelIntersection.iSectType == Intersection::ModelIntersection::Geometry) {
		const Intersection::GeometryIntersection& intersection = modelIntersection.geometryIntersection;

		const Mesh& mesh = model.GetMesh (intersection.mesh);
		const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
		const Material& material = model.GetMaterial (triangle.material);

		Vec3 originalNormal = mesh.GetNormal (intersection.triangle, intersection.position);
		Vec3 rayDirectedNormal = originalNormal;
		if (intersection.facing == Intersection::ShapeIntersection::Back) {
			rayDirectedNormal = rayDirectedNormal * -1.0;
		}

		color += material.GetAmbientColor ();
		for (UIndex i = 0; i < model.LightCount (); i++) {
			const Light& light = model.GetLight (i);
			if (!IsInShadow (intersection.position, light)) {
				const Vec3& photonOrigin = light.GetPosition ();
				color += GetPhongShading (material, light, photonOrigin, intersection.position, rayDirectedNormal, ray.GetDirection ());
			} 			
		}

		if (material.IsReflective ()) {
			Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), rayDirectedNormal);
			InfiniteRay reflectedRay (intersection.position, reflectedDirection);
			Color reflectedColor = Trace (reflectedRay, depth + 1);
			color += reflectedColor * material.GetReflection ();
		}

		if (material.IsTransparent ()) {
			double transparency = material.GetTransparency ();
			double refractionIndex = material.GetRefractionIndex ();
			color = color * (1.0 - transparency);

			Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), originalNormal, refractionIndex);
			InfiniteRay refractedRay (intersection.position, refractedDirection);
			Color refractedColor = Trace (refractedRay, depth + 1);
			color += refractedColor * transparency;
		}
	} else if (modelIntersection.iSectType == Intersection::ModelIntersection::Light) {
		const Intersection::LightIntersection& intersection = modelIntersection.lightIntersection;
		const Light& light = model.GetLight (intersection.light);
		color += light.GetColor ();
	}

	return Clamp (color);
}

bool RayTracer::IsInShadow (const Vec3& position, const Light& light) const
{
	SectorRay shadowRay (position, light.GetPosition ());
	return Intersection::RayGeometry (shadowRay, model, NULL);
}
