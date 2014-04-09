#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "model.hpp"

class Generator
{
public:
	static void GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, int material, bool inverted);
};

#endif
