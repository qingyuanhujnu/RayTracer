#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Vec3& shadedPoint, const Vec3& shadedPointNormal, const Ray& ray)
{
	Vec3 lightDirection = Normalize (light.GetPosition () - shadedPoint);
	Vec3 reflectionVector = Reflect (lightDirection, shadedPointNormal);

	double lightNormalProduct = lightDirection * shadedPointNormal;
	double diffuseCoeff = std::max (lightNormalProduct, 0.0);

	double shininess = material.GetShininess ();
	double specularCoeff = pow (std::max (reflectionVector * ray.GetDirection (), 0.0), shininess);

	Color diffuseColor = light.GetColor () * material.GetDiffuseColor ();
	Color result = diffuseColor * diffuseCoeff + material.GetSpecularColor () * specularCoeff + material.GetAmbientColor ();
	return Clamp (result);
}
