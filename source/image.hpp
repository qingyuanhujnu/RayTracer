#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "camera.hpp"
#include <vector>

class Image
{
public:
	Image (const Camera& camera, int resolutionX, int resolutionY, double distance);
	~Image ();

	Vec3		GetFieldCenter (int x, int y) const;

private:
	double		fieldWidth;
	double		fieldHeight;

	Vec3		xDirection;
	Vec3		yDirection;

	Vec3		bottomLeft;
};

#endif
