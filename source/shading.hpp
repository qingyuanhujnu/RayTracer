#ifndef SHADING_HPP
#define SHADING_HPP

#include "coord.hpp"
#include "material.hpp"
#include "light.hpp"

Color GetPhongShading (const Material& material, const Light& light, const Coord& coordinate, const Coord& normal);

#endif
