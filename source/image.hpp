#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "camera.hpp"
#include <vector>

class Image
{
public:
	Image (const Camera& camera, int resolutionX, int resolutionY, double distance);
	~Image ();

	Coord		GetFieldCenter (int x, int y) const;

private:
	double		fieldWidth;
	double		fieldHeight;

	Coord		xDirection;
	Coord		yDirection;

	Coord		bottomLeft;
};

#endif
