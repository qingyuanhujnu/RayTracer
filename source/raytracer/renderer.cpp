#include "renderer.hpp"
#include "threading.hpp"
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

Renderer::Renderer (const Model& model, const Camera& camera, int sampleNum) :
	model (model),
	camera (camera),
	sampleNum (sampleNum)
{
}

Renderer::~Renderer ()
{
}

class ProgressReport
{
public:
	ProgressReport (const Renderer::IProgress* progress) :
		progress (progress),
		reportInterval (1000),
		finishedPixels (0),
		lastFinishedPixels (0)
	{
	
	}

	void Report (int x, int y, double r, double g, double b, int picWidth, int picHeight)
	{
		progress->OnPixelReady (x, y, r, g, b, picWidth, picHeight);

		lock.Enter ();
		finishedPixels++;
		if (finishedPixels == lastFinishedPixels + reportInterval) {
			progress->OnProgress ((double) finishedPixels / (double) (picWidth * picHeight));
			lastFinishedPixels = finishedPixels;
		}
		lock.Leave ();
	}

private:
	const Renderer::IProgress*	progress;
	int							reportInterval;
	int							finishedPixels;
	int							lastFinishedPixels;
	Lock						lock;
};

class RenderThread : public Thread
{
public:
	RenderThread (int threadIndex,
				  int threadNum,
				  const Image* image,
				  const Renderer* renderer,
				  const Renderer::Parameters* parameters,
				  ProgressReport* progressReport,
				  Renderer::ResultImage* resultImage) :
		Thread (),
		threadIndex (threadIndex),
		threadNum (threadNum),
		image (image),
		renderer (renderer),
		parameters (parameters),
		progressReport (progressReport),
		resultImage (resultImage)
	{
	
	}

	virtual int Do ()
	{
		const int resX = parameters->GetResolutionX ();
		const int resY = parameters->GetResolutionY ();
		for (int pix = threadIndex; pix < (resX * resY); pix += threadNum) {
			int x = pix % resX;
			int y = resY - (pix / resX) - 1;

			Image::Field field = image->GetField (x, y);
			Color fieldColor = renderer->GetFieldColor (field);
			resultImage->SetColor (x, y, fieldColor);

			progressReport->Report (x, y, fieldColor.r, fieldColor.g, fieldColor.b, resX, resY);
		}
		return 0;
	}

private:
	int threadIndex;
	int threadNum;
	const Image* image;
	const Renderer* renderer;
	const Renderer::Parameters* parameters;
	ProgressReport* progressReport;
	Renderer::ResultImage* resultImage;
};

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

	ProgressReport progressReport (&progress);
	progress.OnProgress (0.0);
//#define USEOMP
#ifdef USEOMP
	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();
	const int procs = omp_get_num_procs ();		// logical cores
	#pragma omp parallel for schedule(dynamic, 4) num_threads (procs)
	for (int pix = 0; pix < (resX * resY); ++pix) {
		int x = pix % resX;
		int y = resY - (pix / resX) - 1;

		Image::Field field = image.GetField (x, y);
		Color fieldColor = GetFieldColor (field);
		result.SetColor (x, y, fieldColor);

		#pragma omp critical
		progressReport.Report (x, y, fieldColor.r, fieldColor.g, fieldColor.b, resX, resY);
	}
#else
	const UIndex threadNum = 4;
	std::vector<RenderThread> threads;
	for (UIndex i = 0; i < threadNum; i++) {
		threads.push_back (RenderThread (
			i,
			threadNum,
			&image,
			this,
			&parameters,
			&progressReport,
			&result
		));
	}
	for (UIndex i = 0; i < threadNum; i++) {
		threads[i].Start ();
	}
	for (UIndex i = 0; i < threadNum; i++) {
		threads[i].Wait ();
	}
#endif
	progress.OnProgress (1.0);

	return true;
}
