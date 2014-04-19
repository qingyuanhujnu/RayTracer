#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "common.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "image.hpp"

class Renderer {
public:
	class Parameters
	{
	public:
		Parameters ();
		Parameters (int resolutionX, int resolutionY, double imageDistance);
		~Parameters ();

		void	Set (int resolutionX, int resolutionY, double imageDistance);
		bool	Check () const;

		int		GetResolutionX () const;
		int		GetResolutionY () const;
		double	GetImageDistance () const;

	private:
		int		resolutionX;
		int		resolutionY;
		double	imageDistance;
	};

	class ResultImage
	{
	public:
		ResultImage ();
		~ResultImage ();

		void				SetResolution (int resolutionX, int resolutionY);
		void				SetColor (int x, int y, const Color& color);

		int					GetResolutionX () const;
		int					GetResolutionY () const;
		const Color&		GetColor (int x, int y) const;

	private:
		int					resolutionX;
		int					resolutionY;
		std::vector<Color>	image;
	};

	Renderer (const Model& model, const Camera& camera);
	virtual ~Renderer ();

	virtual bool	Render (const Parameters& parameters, ResultImage& result);

protected:
	virtual Color	GetFieldColor (const Image::Field& field) = 0;

	Model			model;
	Camera			camera;
};

#endif