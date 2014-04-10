#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
public:
	Color ();
	Color (double r, double g, double b);
	~Color ();

	Color	operator+ (const Color& color) const;
	Color	operator- (const Color& color) const;
	Color	operator* (const Color& color) const;
	Color	operator* (double scalar) const;

	Color&	operator+= (const Color& color);

	double	r;
	double	g;
	double	b;
};

Color Clamp (const Color& color);

#endif
