#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, 
					   const Light& light,
					   const Vec3& photonOrigin,
					   const Vec3& shadedPoint,
					   const Vec3& shadedPointNormal,
					   const Vec3& viewDirection)
{
	Vec3 lightDirection = Normalize (light.GetPosition () - shadedPoint);
	Vec3 reflectionVector = GetReflectedDirection (lightDirection, shadedPointNormal);

	double lightNormalProduct = lightDirection * shadedPointNormal;
	double diffuseCoeff = std::max (lightNormalProduct, 0.0);

	double shininess = material.GetShininess ();
	double specularCoeff = pow (std::max (reflectionVector * viewDirection, 0.0), shininess);

	Color diffuseColor = light.GetColor () * material.GetDiffuseColor ();
	Color color = diffuseColor * diffuseCoeff + material.GetSpecularColor () * specularCoeff;

	// attenuation
	double distance = Distance (shadedPoint, photonOrigin);
	double intensity = light.GetIntensity (distance);
	color = color * intensity;

	return Clamp (color);
}
