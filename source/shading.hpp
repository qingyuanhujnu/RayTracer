#ifndef SHADING_HPP
#define SHADING_HPP

#include "Vec3.hpp"
#include "material.hpp"
#include "light.hpp"

Color GetPhongShading (const Material& material, const Light& light, const Vec3& Vec3inate, const Vec3& normal);

#endif
