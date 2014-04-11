#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Vec3& coord, const Vec3& normal)
{
	Color ambientProduct = light.GetAmbientColor () * material.GetAmbientColor ();
	Color diffuseProduct = light.GetDiffuseColor () * material.GetDiffuseColor ();
	Color specularProduct = light.GetSpecularColor () * material.GetSpecularColor ();

	Vec3 lightDirection = Normalize (light.GetPosition () - coord);
	double lightNormalProduct = lightDirection * normal;

	double diffuseCoeff = std::max (lightNormalProduct, 0.0);
	double specularCoeff = pow (std::max (lightNormalProduct, 0.0), 1.0);

	Color result = diffuseProduct * diffuseCoeff + specularProduct * specularCoeff + ambientProduct;
	return Clamp (result);
}
