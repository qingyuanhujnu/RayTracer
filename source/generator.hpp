#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "common.hpp"
#include "model.hpp"

class Generator
{
public:
	static void GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, UIndex material, bool inverted);
	static void GenerateCylinder (Model& model, double radius, double height, int segmentation, const Coord& offset, UIndex material, bool inverted);
};

#endif
