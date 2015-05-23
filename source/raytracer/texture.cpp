#include "texture.hpp"
#include "common.hpp"
#include <fstream>

MonoTexture::MonoTexture (const Color& color) :
	color (color)
{
}

Color MonoTexture::Sample (const Vec2& /*texCoord*/) const
{
	return color;
}

BitMapTexture::BitMapTexture (const std::wstring& texturePath) :
	pixbuff (nullptr),
	width (0),
	height (0),
	bitsPerPixel (0),
	initialized (false)
{
	std::fstream fs;
	fs.open (texturePath, std::ios::in | std::ios::binary);
	if (fs.is_open ())
	{
		char header[54];
		fs.read (header, 54);
		// Check header
		if (header[0] != 0x42 || header[1] != 0x4d) 
		{
			DBGASSERT (false);
			return;
		}

		size_t offset = 0;
		{
			char* t = (char*)&offset;
			t[0] = header[10];
			t[1] = header[11];
			t[2] = header[12];
			t[3] = header[13];
		}
		// already read 54 bytes
		if (offset > 54)
			offset = offset - 54;
		else
			offset = 0;

		{
			char* t = (char*)&width;
			t[0] = header[18];
			t[1] = header[19];
			t[2] = header[20];
			t[3] = header[21];
		}

		{
			char* t = (char*)&height;
			t[0] = header[22];
			t[1] = header[23];
			t[2] = header[24];
			t[3] = header[25];
		}

		{
			char* t = (char*)&bitsPerPixel;
			t[0] = header[28];
			t[1] = header[29];
		}

		if (bitsPerPixel != 24) {
			// not supported
			DBGASSERT (false);
			return;
		}

		size_t image_data_size = 0;
		{
			char* t = (char*)&image_data_size;
			t[0] = header[34];
			t[1] = header[35];
			t[2] = header[36];
			t[3] = header[37];

			DBGASSERT (image_data_size == width * height * bitsPerPixel / 8);
		}

		if (offset > 0) {
			fs.ignore (offset);
		}

		pixbuff = new char[image_data_size];
		fs.read (pixbuff, image_data_size);

		initialized = true;
	}
}

BitMapTexture::~BitMapTexture ()
{
	delete[] pixbuff;
	pixbuff = nullptr;
}

Color BitMapTexture::Sample (const Vec2& texCoord) const
{
	if (!initialized)
		return Color ();

	size_t x = (size_t)(texCoord.x * (width - 1));
	size_t y = (size_t)(texCoord.y * (height - 1));

	// rows go from bottom to top
	y = height - 1 - y;

	// 4 byte padding
	size_t rowSize = ((width * bitsPerPixel/8 + 3) / 4) * 4;
	char* pixel = pixbuff + rowSize * y + x * bitsPerPixel/8;

	// blue, green, red
	unsigned char red = pixel[2];
	unsigned char green = pixel[1];
	unsigned char blue = pixel[0];

	return Color (red / 255.0, green / 255.0, blue / 255.0);
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