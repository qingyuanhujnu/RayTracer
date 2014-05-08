#ifndef SHADING_HPP
#define SHADING_HPP

#include "vec3.hpp"
#include "material.hpp"
#include "light.hpp"
#include "ray.hpp"

Color GetPhongShading (const Material& material, 
					   const Light& light,
					   const Vec3& photonOrigin,
					   const Vec3& shadedPoint,
					   const Vec3& shadedPointNormal,
					   const Vec3& viewDirection);

#endif
