#include "texture.hpp"

MonoTexture::MonoTexture (const Color& color) :
	color (color)
{
}

Color MonoTexture::Sample (const Vec2& /*texCoord*/) const
{
	return color;
}

BitMapTexture::BitMapTexture (const std::wstring& /*texturePath*/)
{
}

Color BitMapTexture::Sample (const Vec2& texCoord) const
{
	// TODO

	return Color (texCoord.x, texCoord.y, 0.0);
}

CheckBoardTexture::CheckBoardTexture (unsigned int xsize, unsigned int ysize) :
	xsize (xsize),
	ysize (ysize)
{
}

Color CheckBoardTexture::Sample (const Vec2& texCoord) const
{
	double squareWidth = 1.0 / xsize;
	double squareHeight = 1.0 / ysize;

	bool xParity = ((int)(texCoord.x / squareWidth)) % 2 == 0;
	bool yParity = ((int)(texCoord.y / squareHeight)) % 2 == 0;

	return ((xParity && yParity) || (!xParity && !yParity)) ? Color (0.0, 0.0, 0.0) : Color (1.0, 1.0, 1.0);
}