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
		Field (const Vec3& fieldBottomLeft, const Vec3& xDirection, const Vec3& yDirection, double fieldWidth, double fieldHeight, int sampleResolution);

		int		SampleCount () const;
		Vec3	GetSample (int index) const;
		Vec3	GetRandomSample () const;

	private:
		Vec3	fieldBottomLeft;
		Vec3	xDirection;
		Vec3	yDirection;

		double	sampleWidth;
		double	sampleHeight;
		int		sampleRes;
	};

	Image (const Camera& camera, int resolutionX, int resolutionY, double distance, int sampleResolution);
	~Image ();

	Field		GetField (int x, int y) const;
	Vec3		GetFieldCenter (int x, int y) const;

private:
	Vec3		bottomLeft;
	Vec3		xDirection;
	Vec3		yDirection;

	double		fieldWidth;
	double		fieldHeight;
	int			sampleRes;
};

#endif
