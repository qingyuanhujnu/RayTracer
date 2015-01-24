#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "color.hpp"
#include "vec2.hpp"
#include <fstream>

class Texture {
public:
	virtual Color Sample (const Vec2& texCoord) const = 0;
	virtual ~Texture ()
	{
	}
};

class MonoTexture : public Texture {
	Color color;

public:
	MonoTexture (const Color& color);

	Color Sample (const Vec2& texCoord) const override;
};

class BitMapTexture : public Texture {
public:
	BitMapTexture (const std::wstring& texturePath);

	Color Sample (const Vec2& texCoord) const override;
};

// For testing purposes
class CheckBoardTexture : public Texture {
	unsigned int xsize, ysize;
public:
	CheckBoardTexture (unsigned int xsize, unsigned int ysize);

	Color Sample (const Vec2& texCoord) const override;
};

#endif