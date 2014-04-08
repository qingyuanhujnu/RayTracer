#include "shading.hpp"

#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Coord& coordinate, const Coord& triangleNormal)
{
	Color ambientProduct = light.GetAmbientColor () * material.GetAmbientColor ();
	Color diffuseProduct = light.GetDiffuseColor () * material.GetDiffuseColor ();
	Color specularProduct = light.GetSpecularColor () * material.GetSpecularColor ();

	Coord lightDirection = Normalize (light.GetPosition () - coordinate);
	double lightNormalProduct = lightDirection * triangleNormal;

	double diffuseCoeff = std::max (lightNormalProduct, 0.0);
	double specularCoeff = pow (std::max (lightNormalProduct, 0.0), 1.0);

	Color result = diffuseProduct * diffuseCoeff + specularProduct * specularCoeff + ambientProduct;
	return Clamp (result);
}
