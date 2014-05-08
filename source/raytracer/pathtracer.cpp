#include "pathtracer.hpp"
#include "image.hpp"
#include "random.hpp"
#include "shading.hpp"
#include "intersection.hpp"
#include <omp.h>
#include <algorithm>

PathTracer::PathTracer (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera),
	sampleNum (sampleNum)
{
}

Color PathTracer::GetFieldColor (const Image::Field& field)
{
	Color fieldColor;
	for (int s = 0; s < sampleNum; ++s) {
		InfiniteRay cameraRay (camera.GetEye (), field.GetRandomSample () - camera.GetEye ());
		Color color = Clamp (Radiance (cameraRay, 0));
		fieldColor += (color / sampleNum);
	}
	return fieldColor;
}

// based on smallpt
Color PathTracer::Radiance (const Ray& ray, int depth) const
{
	Color color;

	Intersection::GeometryIntersection isect;
	if (!Intersection::RayGeometry (ray, model, &isect)) {			// TODO: this is a bug because I need to do a ModelIntersection here
		return color;
	}

	const Mesh& mesh = model.GetMesh (isect.mesh);
	const Mesh::Triangle& triangle = mesh.GetTriangle (isect.triangle);
	const Material& material = model.GetMaterial (triangle.material);
	Vec3 normal = mesh.GetNormal (isect.triangle, isect.position);

	Color colDiffuse = material.GetDiffuseColor ();
	double cDiffIntensity = (colDiffuse.r + colDiffuse.g + colDiffuse.b) / 3.0;

	if (++depth > 5 && cDiffIntensity < random ()) {
		return color;				// russian roulette: what a suicidal path we took!
	}

	color += RayCastTowardsLights (isect.position, normal, material);

	// Diffuse reflection
	// Get random direction in the hemisphere given by hit surface's normal
	double r1 = random () * 2 * PI;		// random angle around
	double r2 = cos (random () * PI * 0.5);		// distance that is weighted towards normals direction
	double r2s = sqrt (r2);

	Vec3 w = normal;
	if (normal * ray.GetDirection () > 0)
		w = w * -1.0;
	Vec3 u = Normalize ((fabs (w.x) > 0.1 ? Vec3 (0, 1, 0) : Vec3 (1, 0, 0)) ^ w);
	Vec3 v = w ^ u;
	Vec3 randomDir = Normalize (u*cos (r1)*r2s + v*sin (r1)*r2s + w*sqrt (1 - r2));		// Random direction in hemisphere

	InfiniteRay shadowRay (isect.position, randomDir);
	Intersection::ModelIntersection shadowRayIsect;
	Intersection::RayModel (shadowRay, model, &shadowRayIsect);

	if (shadowRayIsect.iSectType == Intersection::ModelIntersection::Light) {		// light hit!
		const Light& light = model.GetLight (shadowRayIsect.lightIntersection.light);
		color += GetPhongShading (material, light, isect.position, normal, ray.GetDirection ());
	}
	else if (shadowRayIsect.iSectType == Intersection::ModelIntersection::Geometry) {
		color += Radiance (shadowRay, depth) * cDiffIntensity;
	}

	// Ideal reflection
	if (material.IsReflective ()) {
		Vec3 reflectedDirection = GetReflectedDirection (ray.GetDirection (), normal);
		InfiniteRay reflectedRay (isect.position, reflectedDirection);
		Color reflectedColor = Radiance (reflectedRay, depth + 1);
		double reflection = material.GetReflection ();
		color += (reflectedColor * reflection);
	}

	if (material.IsTransparent ()) {
		double transparency = material.GetTransparency ();
		color = color * (1.0 - transparency);

		Vec3 refractedDirection = GetRefractedDirection (ray.GetDirection (), normal);
		InfiniteRay refractedRay (isect.position, refractedDirection);
		Color refractedColor = Radiance (refractedRay, depth + 1);
		color += refractedColor * transparency;
	}

	return Clamp (color);
}

// Loop over any lights. This code only works for spherical lights right now but it could be modified to work with any bound box. 
// Note that path tracing works without this but shooting rays towards the light's sphere makes it converge in less samples.
Color PathTracer::RayCastTowardsLights (const Vec3& position, const Vec3& normal, const Material& material) const
{
	Color color;

	// Start a random ray toward light sphere.
	for (UIndex i = 0; i < model.LightCount (); ++i) {
		const Light& light = model.GetLight (i);

		Vec3 sw = light.GetPosition () - position;
		Vec3 su = Normalize ((fabs (sw.x) > .1 ? Vec3 (0, 1, 0) : Vec3 (1, 0, 0)) ^ sw);
		Vec3 sv = sw ^ su;
		double lr = light.GetRadius ();
		double cos_a_max = sqrt (1 - lr*lr / ((position - light.GetPosition ()) * (position - light.GetPosition ())));		// angle of the cone in which we see the light
		double eps1 = random (), eps2 = random ();
		double cos_a = 1 - eps1 + eps1*cos_a_max;
		double sin_a = sqrt (1 - cos_a*cos_a);
		double phi = 2 * PI * eps2;
		Vec3 l = su*cos (phi)*sin_a + sv*sin (phi)*sin_a + sw*cos_a;
		l = Normalize (l);

		Vec3 lightIsect;
		InfiniteRay ray (position, l);
		Intersection::ModelIntersection iSect;		
		if (Intersection::RayModel (ray, model, &iSect) && 
			iSect.iSectType == Intersection::ModelIntersection::Light &&
			iSect.lightIntersection.light == i) {		// If hit then also check if we hit the light we intended to hit.
			double omega = 2 * PI * (1 - cos_a_max);
			color += GetPhongShading (material, light, position, normal, ray.GetDirection ()) * omega /** INV_PI*/;	// TODO: I need to research BRDF to decide if the 1/PI is needed here or not.
		}
	}

	return color;
}