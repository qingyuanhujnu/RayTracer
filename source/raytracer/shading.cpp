#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Vec3& coord, const Vec3& normal)
{
	Vec3 lightDirection = Normalize (light.GetPosition () - coord);
	double lightNormalProduct = lightDirection * normal;

	double diffuseCoeff = std::max (lightNormalProduct, 0.0);

	double shininess = material.GetShininess ();
	double specularCoeff = pow (std::max (lightNormalProduct, 0.0), shininess);

	Color diffuseColor = light.GetColor () * material.GetDiffuseColor ();
	Color result = diffuseColor * diffuseCoeff + material.GetSpecularColor () * specularCoeff + material.GetAmbientColor ();
	return Clamp (result);
}
