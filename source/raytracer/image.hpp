#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "camera.hpp"
#include <vector>

class Image
{
public:
	class Field
	{
	public:
		Field (const Vec3& fieldBottomLeft, const Vec3& xDirection, const Vec3& yDirection, double fieldWidth, double fieldHeight);

		Vec3	GetFixSample (int sampleResolution, int index) const;
		Vec3	GetRandomSample () const;

	private:
		Vec3	fieldBottomLeft;
		Vec3	xDirection;
		Vec3	yDirection;

		double	fieldWidth;
		double	fieldHeight;
	};

	Image (const Camera& camera, int resolutionX, int resolutionY, double distance);
	~Image ();

	Field		GetField (int x, int y) const;
	Vec3		GetFieldCenter (int x, int y) const;

private:
	Vec3		bottomLeft;
	Vec3		xDirection;
	Vec3		yDirection;

	double		fieldWidth;
	double		fieldHeight;
};

#endif
