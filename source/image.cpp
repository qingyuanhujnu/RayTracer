#include "image.hpp"
#include "common.hpp"
#include "random.hpp"

#include <math.h>

Image::Field::Field (const Vec3& fieldBottomLeft, const Vec3& xDirection, const Vec3& yDirection, double fieldWidth, double fieldHeight, int sampleResolution) :
	fieldBottomLeft (fieldBottomLeft),
	xDirection (xDirection),
	yDirection (yDirection),
	sampleRes (sampleResolution)
{
	sampleWidth = fieldWidth / sampleRes;
	sampleHeight = fieldHeight / sampleRes;
}

int Image::Field::SampleCount () const
{
	return sampleRes * sampleRes;
}

Vec3 Image::Field::GetSample (int index) const
{
	int x = index % sampleRes;
	int y = index / sampleRes;

	Vec3 result = fieldBottomLeft;
	result = Offset (result, xDirection, x * sampleWidth + sampleWidth / 2.0);
	result = Offset (result, yDirection, y * sampleHeight + sampleHeight / 2.0);
	return result;
}

Image::Image (const Camera& camera, int resolutionX, int resolutionY, double distance, int sampleResolution)
{
	sampleRes = sampleResolution;

	double imageWidth = distance * tan (camera.GetXFov () * DEGRAD) * distance;
	double imageHeight = distance * tan (camera.GetYFov () * DEGRAD) * distance;
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
	return Field (fieldBottomLeft, xDirection, yDirection, fieldWidth, fieldHeight, sampleRes);
}

Vec3 Image::GetFieldCenter (int x, int y) const
{
	Vec3 result = bottomLeft;
	result = Offset (result, xDirection, x * fieldWidth + fieldWidth / 2.0);
	result = Offset (result, yDirection, y * fieldHeight + fieldHeight / 2.0);
	return result;
}

Vec3 Image::GetFieldRandom (int x, int y) const
{
	Vec3 result = bottomLeft;
	result = Offset (result, xDirection, x * fieldWidth + fieldWidth * random ());
	result = Offset (result, yDirection, y * fieldHeight + fieldHeight *random ());
	return result;
}
