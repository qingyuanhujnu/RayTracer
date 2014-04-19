#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Vec3& shadedPoint, const Ray& ray, const Vec3& normal)
{
	Vec3 lightDirection = Normalize (light.GetPosition () - shadedPoint);
	Vec3 reflectionVector = 2*(lightDirection * normal) * normal - lightDirection; // the direction of a perfectly reflected ray from the light

	double lightNormalProduct = lightDirection * normal;
	double diffuseCoeff = std::max (lightNormalProduct, 0.0);

	double shininess = material.GetShininess ();
	Vec3 rayDirInv = -1.0 * ray.GetDirection ();
	double specularCoeff = pow (std::max (reflectionVector * rayDirInv, 0.0), shininess);

	Color diffuseColor = light.GetColor () * material.GetDiffuseColor ();
	Color result = diffuseColor * diffuseCoeff + material.GetSpecularColor () * specularCoeff + material.GetAmbientColor ();
	return Clamp (result);
}
