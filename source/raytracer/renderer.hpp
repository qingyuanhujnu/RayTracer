#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "common.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "image.hpp"
#include "threading.hpp"

class Renderer {
public:
	class IProgress
	{
	public:
		IProgress ();
		virtual ~IProgress ();

		virtual void OnProgress (double progress) const = 0;
		virtual void OnPixelReady (int x, int y, double r, double g, double b, int picWidth, int picHeight) const = 0;
	};

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

	Renderer (const Model& model, const Camera& camera, int sampleNum);
	virtual ~Renderer ();

	virtual bool	Render (const Parameters& parameters, ResultImage& result, const IProgress& progress);
	void	Cancel ();
	bool	IsCanceled () const;
	virtual Color	GetFieldColor (const Image::Field& field) const = 0;

protected:
	Model			model;
	Camera			camera;
	int				sampleNum;

private:
	// Note that canceled is read/write from multiple threads but since there is only one state change (false -> true) in the objects lifetime
	// it doesn't need to be atomic.
	bool canceled;
};

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
			progress->OnProgress ((double)finishedPixels / (double)(picWidth * picHeight));
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

#endif
