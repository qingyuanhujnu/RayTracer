#include "renderer.hpp"
#include <omp.h>

Renderer::IProgress::IProgress ()
{

}

Renderer::IProgress::~IProgress ()
{

}

Renderer::Parameters::Parameters ()
{
	Set (100, 100, 1.0);
}

Renderer::Parameters::Parameters (int resolutionX, int resolutionY, double imageDistance)
{
	Set (resolutionX, resolutionY, imageDistance);
}

Renderer::Parameters::~Parameters ()
{

}

void Renderer::Parameters::Set (int resolutionX, int resolutionY, double imageDistance)
{
	this->resolutionX = resolutionX;
	this->resolutionY = resolutionY;
	this->imageDistance = imageDistance;
}

bool Renderer::Parameters::Check () const
{
	if (resolutionX <= 0 || resolutionY <= 0) {
		return false;
	}
	if (!IsPositive (imageDistance)) {
		return false;
	}
	return true;
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

Renderer::Renderer (const Model& model, const Camera& camera) :
	model (model),
	camera (camera)
{
}

Renderer::~Renderer ()
{
}

bool Renderer::Render (const Parameters& parameters, ResultImage& result, const IProgress& progress)
{
	if (DBGERROR (!parameters.Check ())) {
		return false;
	}

	if (DBGERROR (!model.Check ())) {
		return false;
	}

	if (DBGERROR (model.LightCount () == 0)) {
		return false;
	}

	Image image (camera, parameters.GetResolutionX (), parameters.GetResolutionY (), parameters.GetImageDistance ());
	result.SetResolution (parameters.GetResolutionX (), parameters.GetResolutionY ());

	const int reportInterval = 1000;
	int finishedPixels = 0;
	int lastFinishedPixels = 0;

	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();

	progress.OnProgress (0.0);
#ifndef DEBUG
	const int procs = omp_get_num_procs ();		// logical cores
	#pragma omp parallel for schedule(dynamic, 4) num_threads (procs - 1)
#endif
	for (int pix = 0; pix < (resX * resY); ++pix) {
		int x = pix % resX;
		int y = resY - (pix / resX) - 1;

		Image::Field field = image.GetField (x, y);
		Color fieldColor = GetFieldColor (field);
		result.SetColor (x, y, fieldColor);

		progress.OnPixelReady (x, y, fieldColor.r, fieldColor.g, fieldColor.b, resX, resY);
#ifndef DEBUG
		#pragma omp critical
#endif
		{
			finishedPixels++;

			if (finishedPixels == lastFinishedPixels + reportInterval) {
				progress.OnProgress ((double) finishedPixels / (double) (resX * resY));
				lastFinishedPixels = finishedPixels;
			}
		}
	}
	progress.OnProgress (1.0);

	return true;
}
