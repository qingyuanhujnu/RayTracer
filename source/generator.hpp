#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "mesh.hpp"

class Generator
{
public:
	static void GenerateCuboid (Mesh& mesh, double xSize, double ySize, double zSize, const Coord& offset, int material, bool inverted);
};

#endif
