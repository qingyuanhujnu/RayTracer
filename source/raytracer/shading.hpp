#ifndef SHADING_HPP
#define SHADING_HPP

#include "vec3.hpp"
#include "material.hpp"
#include "light.hpp"
#include "ray.hpp"

Color GetPhongShading (const Material& material, const Light& light, const Vec3& shadedPoint, const Ray& ray, const Vec3& normal);

#endif
