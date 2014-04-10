#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "common.hpp"
#include "model.hpp"

class Generator
{
public:
	enum Facing {
		Inside,
		Outside
	};

	static void GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, UIndex material);
	static void GenerateInsideOutCuboid (Model& model, double xSize, double ySize, double zSize, const Coord& offset, UIndex material);
	static void GenerateCylinder (Model& model, double radius, double height, int segmentation, const Coord& offset, UIndex material);
};

#endif
