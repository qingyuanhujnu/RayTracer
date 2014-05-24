#include "pathtracer2.hpp"
#include "image.hpp"
#include "random.hpp"
#include "shading.hpp"
#include "intersection.hpp"
#include "average.hpp"

PathTracer2::PathTracer2 (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera, sampleNum)
{
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

		const Mesh& mesh = model.GetMesh (intersection.mesh);
		const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
		const Material& material = model.GetMaterial (triangle.material);
		Vec3 normal = mesh.GetNormal (intersection.triangle, intersection.position);
		if (intersection.facing == Intersection::ShapeIntersection::Back) {
			normal = normal * -1.0;
		}

		double diffuseCompensation = 0.6; // https://www.youtube.com/watch?v=YWf5BLUOhNM
		Color diffuseColor = material.GetDiffuseColor ();
		double diffuseIntensity = ((diffuseColor.r + diffuseColor.g + diffuseColor.b) / 3.0) * diffuseCompensation;
		color += SampleLights (material, intersection.position, normal, ray.GetDirection ()) * (1.0 - diffuseIntensity);
		color += SampleGeometry (intersection.position, normal, depth) * diffuseIntensity;

		if (material.IsReflective ()) {
			Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
			InfiniteRay reflectedRay (intersection.position, reflectedDirection);
			Color reflectedColor = Trace (reflectedRay, depth + 1);
			color += reflectedColor * material.GetReflection ();
		}

		if (material.IsTransparent ()) {
			double transparency = material.GetTransparency ();
			double refractionIndex = material.GetRefractionIndex ();
			color = color * (1.0 - transparency);

			Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), normal, refractionIndex);
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

Color PathTracer2::SampleLights (const Material& material, const Vec3& point, const Vec3& normal, const Vec3& viewDirection) const
{
	Color color = material.GetAmbientColor ();
	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		Vec3 randomLightPoint = light.GetRandomPoint ();
		SectorRay lightRay (point, randomLightPoint);

		if (!Intersection::RayGeometry (lightRay, model, NULL)) {
			color += GetPhongShading (material, light, randomLightPoint, point, normal, viewDirection);
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
	Color color;
	Vec3 randomDir = RandomDirectionOnHemisphere (normal);
	InfiniteRay lightRay (point, randomDir);
	Intersection::ModelIntersection modelIntersection;
	if (Intersection::RayModel (lightRay, model, &modelIntersection) &&
		modelIntersection.iSectType == Intersection::ModelIntersection::Geometry)	{
		color = Trace (lightRay, depth + 1);
	}
	return Clamp (color);
}
