#ifndef GPUTRACER_HPP
#define GPUTRACER_HPP

#include "gpumodel.hpp"

#ifdef OPENCL

#include "CL/cl.h"
#include "Renderer.hpp"

class GPUTracer : public Renderer {
private:
	bool initialized;

	cl_kernel kernel;
	cl_command_queue command_queue;
	cl_context context;

	std::vector<CL_Triangle> serializedGeometry;
	cl_mem serializedGeometry_device;

	std::vector<CL_Light> serializedLights;
	cl_mem serializedLights_device;

	bool InitOpenCL ();
	bool SerializeModel ();
	bool SerializeGeometry ();
	bool SerializeLights ();

	struct Rect {
		int x, y;
		int width, height;
	};

	bool	RenderImageRect (const Parameters& parameters, ResultImage& result, const Image& image, ProgressReport& progressReport, const Rect& rect) const;
	bool	RenderOnTheGPU (const std::vector<CL_Ray>& rays, std::vector<Color>& result) const;

public:
	GPUTracer (const Model& model, const Camera& camera, int sampleNum);

	bool	Render (const Parameters& parameters, ResultImage& result, const IProgress& progress) override;
	Color	GetFieldColor (const Image::Field& field) const override;

	~GPUTracer () override;
};

#endif

#endif