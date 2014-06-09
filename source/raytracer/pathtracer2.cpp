#include "pathtracer2.hpp"
#include "image.hpp"
#include "random.hpp"
#include "shading.hpp"
#include "intersection.hpp"
#include "average.hpp"

PathTracer2::PathTracer2 (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera, sampleNum),
	hasTransparency (false)
{
	for (UIndex i = 0; i < model.MaterialCount (); i++) {
		const Material& material = model.GetMaterial (i);
		if (material.IsTransparent ()) {
			hasTransparency = true;
			break;
		}
	}
}

Color PathTracer2::GetFieldColor (const Image::Field& field) const
{
	Average<Color> averageColor;
	for (int i = 0; i < sampleNum; i++) {
		if (!IsZero (camera.GetApertureRadius ()) && !IsZero (camera.GetFocalLength ())) {
			Vec3 aperturePosition = camera.GetRandomAperturePoint (field.GetXDirection (), field.GetYDirection ());
			Vec3 samplePosition = field.GetRandomSample ();
			Vec3 focalPoint = camera.GetFocalPoint (samplePosition);
			InfiniteRay cameraRay (aperturePosition, focalPoint - aperturePosition);
			averageColor.Add (Trace (cameraRay, 0));
		} else {
			InfiniteRay cameraRay (camera.GetEye (), field.GetRandomSample () - camera.GetEye ());
			averageColor.Add (Trace (cameraRay, 0));
		}
	}
	return averageColor.Get ();
}

Color PathTracer2::Trace (const Ray& ray, int depth) const
{
	Color color;
	if (depth > 5) {
		return color;
	}

	Intersection::ModelIntersection modelIntersection;
	if (!Intersection::RayModel (ray, model, &modelIntersection)) {
		return color;
	}

	if (modelIntersection.iSectType == Intersection::ModelIntersection::Geometry) {
		const Intersection::GeometryIntersection& intersection = modelIntersection.geometryIntersection;
		color += TraceGeometry (intersection, ray.GetDirection (), depth);
	} else if (modelIntersection.iSectType == Intersection::ModelIntersection::Light) {
		const Intersection::LightIntersection& intersection = modelIntersection.lightIntersection;
		color += TraceLight (intersection);
	}

	return Clamp (color);
}

Color PathTracer2::TraceGeometry (const Intersection::GeometryIntersection& intersection, const Vec3& rayDirection, int depth) const
{
	Color color;

	const Mesh& mesh = model.GetMesh (intersection.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	Vec3 originalNormal = mesh.GetNormal (intersection.triangle, intersection.position);
	Vec3 rayDirectedNormal = originalNormal;
	if (intersection.facing == Intersection::ShapeIntersection::Back) {
		rayDirectedNormal = rayDirectedNormal * -1.0;
	}

	color += material.GetAmbientColor ();

	const double lightSampleProbability = 0.5;
	if (Random () < lightSampleProbability) {
		color += SampleLights (material, intersection.position, rayDirectedNormal, rayDirection, depth);
	} else {
		Color diffuseColor = material.GetDiffuseColor ();
		double diffuseIntensity = ((diffuseColor.r + diffuseColor.g + diffuseColor.b) / 3.0);
		color += SampleGeometry (intersection.position, rayDirectedNormal, depth) * diffuseIntensity;
	}

	if (material.IsReflective ()) {
		Vec3 reflectedDirection = GetReflectedDirection (rayDirection, rayDirectedNormal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = Trace (reflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	if (material.IsTransparent ()) {
		double transparency = material.GetTransparency ();
		double refractionIndex = material.GetRefractionIndex ();
		color = color * (1.0 - transparency);

		Vec3 refractedDirection = GetRefractedDirection (rayDirection, originalNormal, refractionIndex);
		InfiniteRay refractedRay (intersection.position, refractedDirection);
		Color refractedColor = Trace (refractedRay, depth + 1);
		color += refractedColor * transparency;
	}

	return color;
}

Color PathTracer2::TraceLight (const Intersection::LightIntersection& intersection) const
{
	const Light& light = model.GetLight (intersection.light);
	return light.GetColor ();
}

Color PathTracer2::SampleLights (const Material& material, const Vec3& point, const Vec3& normal, const Vec3& viewDirection, int depth) const
{
	Color color;
	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		Vec3 randomLightPoint = light.GetRandomPoint ();
		SectorRay lightRay (point, randomLightPoint);

		if (hasTransparency) {
			Intersection::GeometryIntersection intersection;
			if (!Intersection::RayGeometry (lightRay, model, &intersection)) {
				color += GetPhongShading (material, light, randomLightPoint, point, normal, viewDirection);
			} else {
				const Mesh& mesh = model.GetMesh (intersection.mesh);
				const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
				const Material& material = model.GetMaterial (triangle.material);
				if (material.IsTransparent () && intersection.facing == Intersection::ShapeIntersection::Front) {
					color += TraceGeometry (intersection, lightRay.GetDirection (), depth + 1);
				}
			}
		} else {
			if (!Intersection::RayGeometry (lightRay, model, NULL)) {
				color += GetPhongShading (material, light, randomLightPoint, point, normal, viewDirection);
			}
		}
	}
	return Clamp (color);
}

Vec3 RandomDirectionOnHemisphere (const Vec3& normal)
{
    double theta = acos (sqrt (1.0 - Random ()));
    double phi = 2.0 * PI * Random ();

    double xs = sin (theta) * cos (phi);
    double ys = cos (theta);
    double zs = sin (theta) * sin (phi);

    Vec3 yVector (normal.x, normal.y, normal.z);
    Vec3 h = yVector;
	if (IsLowerOrEqual (abs (h.x), abs (h.y)) && IsLowerOrEqual (abs (h.x), abs (h.z))) {
        h.x = 1.0;
	} else if (IsLowerOrEqual (fabs (h.y), fabs (h.x)) && IsLowerOrEqual (fabs (h.y), fabs (h.z))) {
        h.y = 1.0;
	} else {
        h.z = 1.0;
	}

    Vec3 xVector = Normalize (h ^ yVector);
    Vec3 zVector = Normalize (xVector ^ yVector);

    Vec3 direction = xs * xVector + ys * yVector + zs * zVector;
    return Normalize (direction);
}

Color PathTracer2::SampleGeometry (const Vec3& point, const Vec3& normal, int depth) const
{
	Vec3 randomDir = RandomDirectionOnHemisphere (normal);
	InfiniteRay randomRay (point, randomDir);
	Color color = Trace (randomRay, depth + 1);
	return Clamp (color);
}
