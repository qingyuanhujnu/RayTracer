#include "color.hpp"

Color::Color () :
	r (0),
	g (0),
	b (0)
{
}

Color::Color (double r, double g, double b) :
	r (r),
	g (g),
	b (b)
{
}

Color::~Color ()
{
}

Color Color::operator+ (const Color& color) const
{
	Color result;
	result.r = r + color.r;
	result.g = g + color.g;
	result.b = b + color.b;
	return result;
}

Color Color::operator- (const Color& color) const
{
	Color result;
	result.r = r - color.r;
	result.g = g - color.g;
	result.b = b - color.b;
	return result;
}

Color Color::operator* (const Color& color) const
{
	Color result;
	result.r = r * color.r;
	result.g = g * color.g;
	result.b = b * color.b;
	return result;
}

Color Color::operator* (double scalar) const
{
	Color result;
	result.r = r * scalar;
	result.g = g * scalar;
	result.b = b * scalar;
	return result;
}

Color Color::operator/ (double scalar) const
{
	Color result;
	result.r = r / scalar;
	result.g = g / scalar;
	result.b = b / scalar;
	return result;
}

Color& Color::operator+= (const Color& color)
{
	r += color.r;
	g += color.g;
	b += color.b;
	return *this;
}

bool Color::operator== (const Color& color) const
{
	return r == color.r && g == color.g && b == color.b;
}

bool Color::operator!= (const Color& color) const
{
	return !this->operator== (color);
}

static double ClampValue (double val)
{
	if (val < 0.0) {
		return 0.0;
	} else if (val > 1.0) {
		return 1.0;
	}
	return val;
}

Color Clamp (const Color& color)
{
	Color result;
	result.r = ClampValue (color.r);
	result.g = ClampValue (color.g);
	result.b = ClampValue (color.b);
	return result;
}
