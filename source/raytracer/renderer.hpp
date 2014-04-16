#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "common.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"

class Renderer {
public:
	class Parameters
	{
	public:
		Parameters (int resolutionX, int resolutionY, double imageDistance);
		~Parameters ();

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

	Renderer (const Model& model, const Camera& camera, const Light& light);
	virtual ~Renderer ();

	virtual void	Render (const Parameters& parameters, ResultImage& result) = 0;

protected:
	Model			model;
	Camera			camera;
	Light			light;
};

#endif