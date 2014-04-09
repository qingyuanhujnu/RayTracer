#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "geometry.hpp"

class Generator
{
public:
	static void GenerateCuboid (Geometry& geometry, double xSize, double ySize, double zSize, const Coord& offset, int material, bool inverted);
};

#endif
