#ifndef GPUTRACER_HPP
#define GPUTRACER_HPP

//#define OPENCL
#ifdef OPENCL

#include "CL/cl.h"
#include "Renderer.hpp"

struct CL_Vec4 {
	cl_float x, y, z, w;

	CL_Vec4 () : x (0.f), y (0.f), z (0.f), w (1.0f) {}
	CL_Vec4 (float x, float y, float z) : x (x), y (y), z (z), w (1.0) {}
};

struct CL_Ray {
	CL_Vec4 origin;
	CL_Vec4 direction;

	CL_Ray (const Vec3& orig, const Vec3& dir) {
		origin.x = (cl_float)orig.x;
		origin.y = (cl_float)orig.y;
		origin.z = (cl_float)orig.z;

		direction.x = (cl_float)dir.x;
		direction.y = (cl_float)dir.y;
		direction.z = (cl_float)dir.z;
	}
};

struct CL_Triangle {
	CL_Vec4 a, b, c;
};

class GPUTracer : public Renderer {
private:
	bool initialized;

	cl_kernel kernel;
	cl_command_queue command_queue;
	cl_context context;

	std::vector<CL_Triangle> serializedModel;
	cl_mem serializedModel_device;

	void InitOpenCL ();
	void SerializeModel ();

public:
	GPUTracer (const Model& model, const Camera& camera, int sampleNum);

	bool	Render (const Parameters& parameters, ResultImage& result, const IProgress& progress) override;
	bool	RenderOnTheGPU (const std::vector<CL_Ray>& rays, std::vector<Color>& result);
	Color	GetFieldColor (const Image::Field& field) const override;

	~GPUTracer () override;
};

#endif

#endif