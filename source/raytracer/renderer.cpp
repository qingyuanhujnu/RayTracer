#include "renderer.hpp"

Renderer::Parameters::Parameters (int resolutionX, int resolutionY, double imageDistance) :
resolutionX (resolutionX),
resolutionY (resolutionY),
imageDistance (imageDistance)
{

}

Renderer::Parameters::~Parameters ()
{

}

int Renderer::Parameters::GetResolutionX () const
{
	return resolutionX;
}

int Renderer::Parameters::GetResolutionY () const
{
	return resolutionY;
}

double Renderer::Parameters::GetImageDistance () const
{
	return imageDistance;
}

Renderer::ResultImage::ResultImage () :
resolutionX (0),
resolutionY (0)
{

}

Renderer::ResultImage::~ResultImage ()
{

}

void Renderer::ResultImage::SetResolution (int inResolutionX, int inResolutionY)
{
	resolutionX = inResolutionX;
	resolutionY = inResolutionY;
	image.clear ();
	for (int i = 0; i < resolutionX * resolutionY; i++) {
		image.push_back (Color ());
	}
}

void Renderer::ResultImage::SetColor (int x, int y, const Color& color)
{
	image[y * resolutionX + x] = color;
}

int Renderer::ResultImage::GetResolutionX () const
{
	return resolutionX;
}

int Renderer::ResultImage::GetResolutionY () const
{
	return resolutionY;
}

const Color& Renderer::ResultImage::GetColor (int x, int y) const
{
	return image[y * resolutionX + x];
}

Renderer::Renderer (const Model& model, const Camera& camera, const Light& light) :
	model (model),
	camera (camera),
	light (light)
{
}

Renderer::~Renderer ()
{
}