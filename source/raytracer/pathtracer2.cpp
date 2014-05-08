#include "pathtracer2.hpp"
#include "image.hpp"
#include "random.hpp"
#include "shading.hpp"
#include "intersection.hpp"
#include "average.hpp"

PathTracer2::PathTracer2 (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera),
	sampleNum (sampleNum)
{
}

Color PathTracer2::GetFieldColor (const Image::Field& field)
{
	Average<Color> averageColor;
	for (int i = 0; i < sampleNum; i++) {
		InfiniteRay cameraRay (camera.GetEye (), field.GetRandomSample () - camera.GetEye ());
		averageColor.Add (Trace (cameraRay, 0));
	}
	return averageColor.Get ();
}

Color PathTracer2::Trace (const Ray& ray, int depth)
{
	Color color;
	if (depth > 5) {
		return color;
	}

	Intersection::GeometryIntersection intersection;
	if (!Intersection::RayGeometry (ray, model, &intersection)) {
		return color;
	}

	const Mesh& mesh = model.GetMesh (intersection.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (intersection.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	Vec3 normal = mesh.GetNormal (intersection.triangle, intersection.position);
	if (intersection.facing == Intersection::ShapeIntersection::Back) {
		normal = normal * -1.0;
	}

	double materialEmission = 0.4;
	color += SampleLights (material, intersection.position, normal, ray.GetDirection ()) * (1.0 - materialEmission);
	color += SampleGeometry (intersection.position, normal, depth) * materialEmission;

	if (material.IsReflective ()) {
		Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (intersection.position, reflectedDirection);
		Color reflectedColor = Trace (reflectedRay, depth + 1);
		color += reflectedColor * material.GetReflection ();
	}

	if (material.IsTransparent ()) {
		double transparency = material.GetTransparency ();
		color = color * (1.0 - transparency);

		Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), normal);
		InfiniteRay refractedRay (intersection.position, refractedDirection);
		Color refractedColor = Trace (refractedRay, depth + 1);
		color += refractedColor * transparency;
	}

	return Clamp (color);
}

static double RandomInRange (double min, double max)
{
    return min + random () * (max - min);
}

static Vec3 RandomPointInSphereVolume (const Vec3& origin, double radius)
{
	double x = RandomInRange (-1.0, 1.0);
	double y = RandomInRange (-1.0, 1.0);
	double z = RandomInRange (-1.0, 1.0);

	Vec3 randomDirection =  Vec3 (x, y, z) * (1.0 / sqrt (x * x + y * y + z * z));
	double randomDistance = RandomInRange (0.0, radius);
	
	Vec3 randomPoint = Offset (origin, randomDirection, randomDistance);
	return randomPoint;
}

Color PathTracer2::SampleLights (const Material& material, const Vec3& point, const Vec3& normal, const Vec3& viewDirection)
{
	Color color;
	for (UIndex i = 0; i < model.LightCount (); i++) {
		const Light& light = model.GetLight (i);
		Vec3 randomLightPoint = RandomPointInSphereVolume (light.GetPosition (), light.GetRadius ());
		SectorRay lightRay (point, randomLightPoint);

		Color shadedColor = material.GetAmbientColor ();
		if (!Intersection::RayGeometry (lightRay, model, NULL)) {
			shadedColor += GetPhongShading (material, light, point, normal, viewDirection);
		}

		double distance = Distance (point, randomLightPoint);
		double intensity = light.GetIntensity (distance);
		shadedColor = shadedColor * intensity;

		color += shadedColor;
	}
	return Clamp (color);
}

Vec3 RandomDirectionOnHemisphere (const Vec3& normal)
{
    double theta = acos (sqrt (1.0 - random ()));
    double phi = 2.0 * PI * random ();

    double xs = sin (theta) * cos(phi);
    double ys = cos (theta);
    double zs = sin (theta) * sin(phi);

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

Color PathTracer2::SampleGeometry (const Vec3& point, const Vec3& normal, int depth)
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
