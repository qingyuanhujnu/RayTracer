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

	static void GenerateRectangle (Model& model, double xSize, double ySize, const Vec3& offset, const Vec3& rotation, UIndex material);
	static void GenerateCuboid (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material);
	static void GenerateInsideOutCuboid (Model& model, double xSize, double ySize, double zSize, const Vec3& offset, const Vec3& rotation, UIndex material);
	static void GenerateCylinder (Model& model, double radius, double height, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material);
	static void GenerateSphere (Model& model, double radius, int segmentation, const Vec3& offset, const Vec3& rotation, UIndex material);
};

#endif
