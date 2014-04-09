#include "shading.hpp"

#include <algorithm>
#include <vector>

Color GetPhongShading (const Material& material, const Light& light, const Coord& coordinate, const Coord& normal)
{
	Color ambientProduct = light.GetAmbientColor () * material.GetAmbientColor ();
	Color diffuseProduct = light.GetDiffuseColor () * material.GetDiffuseColor ();
	Color specularProduct = light.GetSpecularColor () * material.GetSpecularColor ();

	Coord lightDirection = Normalize (light.GetPosition () - coordinate);
	double lightNormalProduct = lightDirection * normal;

	double diffuseCoeff = std::max (lightNormalProduct, 0.0);
	double specularCoeff = pow (std::max (lightNormalProduct, 0.0), 1.0);

	Color result = diffuseProduct * diffuseCoeff + specularProduct * specularCoeff + ambientProduct;
	return Clamp (result);
}

Color GetBRDFShading (const Material& material, const Light& light, const Coord& cameraPos, const Coord& coordinate, const Coord& normal)
{
	const Coord lightAttenuation (0.2, 0.2, 0.02);		// TODO: light should have an attenuation attribute!
	const int shininess = 20;							// TODO: material shoudl have a shininess attribute!

	Color ambientProduct = light.GetAmbientColor () * material.GetAmbientColor ();
	Color diffuseProduct = light.GetDiffuseColor () * material.GetDiffuseColor ();
	Color specularProduct = light.GetSpecularColor () * material.GetSpecularColor ();

	Coord lightDirection = Normalize (light.GetPosition () - coordinate);
	double lightNormalProduct = lightDirection * normal;
	double diffuseCoeff = std::max (lightNormalProduct, 0.0);

	Coord light2Point = Normalize (light.GetPosition () - coordinate);
	Coord point2Cam = Normalize (coordinate - cameraPos);
	Coord halfVec = (light2Point + point2Cam) * 0.5;
	double specularCoeff = pow ((std::max (normal * halfVec, 1.0)), shininess);

	double dist = Distance (coordinate, light.GetPosition ());
	double attenuation = 1.0f / (lightAttenuation.x + dist * lightAttenuation.y + dist * dist * lightAttenuation.z);

	Color result = (diffuseProduct * diffuseCoeff + specularProduct * specularCoeff + ambientProduct) * attenuation;
	return result;
}
