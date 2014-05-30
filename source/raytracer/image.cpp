#include "image.hpp"
#include "common.hpp"
#include "random.hpp"

#include <math.h>

Image::Field::Field (const Vec3& fieldBottomLeft, const Vec3& xDirection, const Vec3& yDirection, double fieldWidth, double fieldHeight) :
	fieldBottomLeft (fieldBottomLeft),
	xDirection (xDirection),
	yDirection (yDirection),
	fieldWidth (fieldWidth),
	fieldHeight (fieldHeight)
{

}

Vec3 Image::Field::GetFixSample (int sampleResolution, int index) const
{
	int x = index % sampleResolution;
	int y = index / sampleResolution;

	double sampleWidth = fieldWidth / sampleResolution;
	double sampleHeight = fieldHeight / sampleResolution;

	Vec3 result = fieldBottomLeft;
	result = Offset (result, xDirection, x * sampleWidth + sampleWidth / 2.0);
	result = Offset (result, yDirection, y * sampleHeight + sampleHeight / 2.0);
	return result;
}

Vec3 Image::Field::GetRandomSample () const
{
	Vec3 result = fieldBottomLeft;
	result = Offset (result, xDirection, fieldWidth * Random ());
	result = Offset (result, yDirection, fieldHeight * Random ());
	return result;
}

const Vec3& Image::Field::GetXDirection () const
{
	return xDirection;
}

const Vec3& Image::Field::GetYDirection () const
{
	return yDirection;
}

Image::Image (const Camera& camera, int resolutionX, int resolutionY, double distance)
{
	double imageWidth = 2.0 * distance * tan (camera.GetXFov () / 2.0);
	double imageHeight = 2.0 * distance * tan (camera.GetYFov () / 2.0);
	fieldWidth = imageWidth / (double) resolutionX;
	fieldHeight = imageHeight / (double) resolutionY;

	Vec3 eyeToCenter = Normalize (camera.GetCenter () - camera.GetEye ());
	xDirection = Normalize (eyeToCenter ^ camera.GetUp ());
	yDirection = Normalize (xDirection ^ eyeToCenter);

	Vec3 pyramidBottom = Offset (camera.GetEye (), eyeToCenter, distance);
	bottomLeft = pyramidBottom;
	bottomLeft = Offset (bottomLeft, xDirection * -1.0, imageWidth / 2.0);
	bottomLeft = Offset (bottomLeft, yDirection * -1.0, imageHeight / 2.0);
}

Image::~Image ()
{

}

Image::Field Image::GetField (int x, int y) const
{
	Vec3 fieldBottomLeft = bottomLeft;
	fieldBottomLeft = Offset (fieldBottomLeft, xDirection, x * fieldWidth);
	fieldBottomLeft = Offset (fieldBottomLeft, yDirection, y * fieldHeight);
	return Field (fieldBottomLeft, xDirection, yDirection, fieldWidth, fieldHeight);
}

Vec3 Image::GetFieldCenter (int x, int y) const
{
	Vec3 result = bottomLeft;
	result = Offset (result, xDirection, x * fieldWidth + fieldWidth / 2.0);
	result = Offset (result, yDirection, y * fieldHeight + fieldHeight / 2.0);
	return result;
}
