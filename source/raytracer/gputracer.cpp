#include "gputracer.hpp"

#ifdef OPENCL

#include <iostream>
#include <fstream>
#include <algorithm>
#include "intersection.hpp"
#include "average.hpp"

GPUTracer::GPUTracer (const Model& model, const Camera& camera, int sampleNum) :
	Renderer (model, camera, sampleNum),
	initialized (false)
{
	bool success;	
	success = InitOpenCL ();
	
	if (success)
		SerializeModel ();
}

// Shamelessly stolen from AMD samples
/* convert the kernel file into a string */
static bool convertToString (const std::wstring& filename , std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f (filename.c_str (), (std::fstream::in | std::fstream::binary));

	if (f.is_open ())
	{
		size_t fileSize;
		f.seekg (0, std::fstream::end);
		size = fileSize = (size_t)f.tellg ();
		f.seekg (0, std::fstream::beg);
		str = new char[size + 1];
		if (!str)
		{
			f.close ();
			return true;
		}

		f.read (str, fileSize);
		f.close ();
		str[size] = '\0';
		s = str;
		delete[] str;
		return true;
	}
	std::cout << "Error: failed to open file\n:" << filename.c_str () << std::endl;
	return false;
}

bool GPUTracer::InitOpenCL ()
{
	if (DBGERROR (initialized)) {
		return false;
	}

	cl_int error = 0;

	// Get the number of platforms on this machine.
	cl_uint numPlatforms;
	clGetPlatformIDs (0, nullptr, &numPlatforms);

	// Get the platform ids.
	std::vector<cl_platform_id> platform_ids;
	platform_ids.resize (numPlatforms);
	clGetPlatformIDs (numPlatforms, &platform_ids[0], NULL);

	// Try to get the GPU device.
	std::vector<cl_device_id> device_ids;
	for (cl_uint i = 0; i < numPlatforms; ++i) {
		cl_uint numDevices = 0;
		clGetDeviceIDs (platform_ids[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices);

		if (numDevices > 0) {
			std::cout << "GPU detected!" << std::endl;
			device_ids.resize (numDevices);
			clGetDeviceIDs (platform_ids[i], CL_DEVICE_TYPE_GPU, numDevices, &device_ids[0], nullptr);
			break;
		}
	}

	// TODO: int this case we could just work on the CPU (if the machine has one ;))
	if (device_ids.size () < 1) {
		std::cout << "Couldn't find GPU" << std::endl;
		return false;
	}

	// Create the context, command queue, and build the kernel.
	context = clCreateContext (nullptr, 1, &device_ids[0], nullptr, nullptr, nullptr);
	command_queue = clCreateCommandQueue (context, device_ids[0], 0, NULL);

	std::string sourceStr;
	if (!convertToString (L"RayTrace.cl", sourceStr))
	{
		std::cout << "Failed to read OpenCL program" << std::endl;
		return false;
	}
	const char *source = sourceStr.c_str ();
	size_t sourceSize[] = { strlen (source) };

	cl_program program = clCreateProgramWithSource (context, 1, &source, sourceSize, nullptr);
	error = clBuildProgram (program, 1, &device_ids[0], nullptr, nullptr, nullptr);
	if (error != CL_SUCCESS) {
		std::cout << "Program Build failed\n";
		size_t logLength = 0;
		clGetProgramBuildInfo (program, device_ids[0], CL_PROGRAM_BUILD_LOG, 0, nullptr, &logLength);
		char* buffer = new char[logLength];
		clGetProgramBuildInfo (program, device_ids[0], CL_PROGRAM_BUILD_LOG, logLength, buffer, &logLength);
		std::cout << "--- Build log ---\n " << buffer << std::endl;
		delete[] buffer;
		return false;
	}

	kernel = clCreateKernel (program, "get_field_color", &error);
	if (error != CL_SUCCESS) {
		std::cout << "Failed to create kernel" << std::endl;
		return false;
	}

	initialized = true;
	return true;
}

bool GPUTracer::SerializeModel ()
{
	if (!SerializeGeometry ())
		return false;

	if (!SerializeLights ())
		return false;

	return true;
}

bool GPUTracer::SerializeGeometry ()
{
	if (DBGERROR (!initialized)) {
		return false;
	}

	for (UIndex meshidx = 0; meshidx < model.MeshCount (); ++meshidx) {
		const Mesh& mesh = model.GetMesh (meshidx);

		for (UIndex triidx = 0; triidx < mesh.TriangleCount (); ++triidx) {
			const Mesh::Triangle& triangle = mesh.GetTriangle (triidx);

			const Mesh::Vertex& v0 = mesh.GetVertex (triangle.vertex0);
			const Mesh::Vertex& v1 = mesh.GetVertex (triangle.vertex1);
			const Mesh::Vertex& v2 = mesh.GetVertex (triangle.vertex2);

			CL_Triangle clTriangle;
			clTriangle.a = CL_Vec4 (v0.pos);
			clTriangle.b = CL_Vec4 (v1.pos);
			clTriangle.c = CL_Vec4 (v2.pos);

			const Vec3 n0 = mesh.GetNormal (triidx, v0.pos);
			clTriangle.na = CL_Vec4 (n0);
			const Vec3 n1 = mesh.GetNormal (triidx, v1.pos);
			clTriangle.nb = CL_Vec4 (n1);
			const Vec3 n2 = mesh.GetNormal (triidx, v2.pos);
			clTriangle.nc = CL_Vec4 (n2);

			serializedGeometry.push_back (clTriangle);
		}
	}

	cl_int error = 0;
	// Upload geometry to device.
	cl_int triangle_count = serializedGeometry.size ();
	serializedGeometry_device = clCreateBuffer (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, triangle_count * sizeof (CL_Triangle), (void*)&serializedGeometry[0], &error);

	error |= clSetKernelArg (kernel, 2, sizeof (cl_mem), &serializedGeometry_device);
	error |= clSetKernelArg (kernel, 3, sizeof (cl_int), &triangle_count);

	return error == CL_SUCCESS;
}

bool GPUTracer::SerializeLights ()
{
	if (DBGERROR (!initialized)) {
		return false;
	}

	for (UIndex lightidx = 0; lightidx < model.LightCount (); ++lightidx) {
		const Light& light = model.GetLight (lightidx);

		CL_Light clLight;
		clLight.pos = CL_Vec4 (light.GetPosition ());
		const Color& color = light.GetColor ();
		clLight.color = CL_Vec4 ((float)color.r, (float)color.g, (float)color.b);

		serializedLights.push_back (clLight);
	}

	cl_int error = 0;
	// Upload lights to device.
	cl_int light_count = serializedLights.size ();
	serializedLights_device = clCreateBuffer (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, light_count * sizeof (CL_Light), (void*)&serializedLights[0], &error);

	error |= clSetKernelArg (kernel, 4, sizeof (cl_mem), &serializedLights_device);
	error |= clSetKernelArg (kernel, 5, sizeof (cl_int), &light_count);

	return error == CL_SUCCESS;
}

bool GPUTracer::Render (const Parameters& parameters, ResultImage& result, const IProgress& progress)
{
	if (DBGERROR (!initialized)) {
		return false;
	}

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

	std::vector<CL_Ray> cl_rays;

	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();
	
	const int renderRectWidth = 128;
	const int renderRectHeight = 128;

	for (int x = 0; x < resX; x += renderRectWidth) {
		for (int y = 0; y < resY; y += renderRectHeight) {
			GPUTracer::Rect rect;
			rect.x = x;
			rect.y = y;
			rect.width = std::min (resX - x, renderRectWidth);
			rect.height = std::min (resY - y, renderRectHeight);

			RenderImageRect (parameters, result, image, progressReport, rect);
		}
	}

	progress.OnProgress (1.0);

	return true;
}

bool GPUTracer::RenderImageRect (const Parameters& parameters, ResultImage& result, const Image& image, ProgressReport& progressReport, const GPUTracer::Rect& rect) const
{
	std::vector<CL_Ray> cl_rays;
	const int sampleRes = (int)sqrt ((double)sampleNum);
	const int resX = parameters.GetResolutionX ();
	const int resY = parameters.GetResolutionY ();

	for (int i = 0; i < rect.height; ++i) {
		for (int j = 0; j < rect.width; ++j) {
			int x = rect.x + j;
			int y = rect.y + i;
			
			Image::Field field = image.GetField (x, y);
			for (int s = 0; s < sampleRes * sampleRes; s++) {
				CL_Ray cameraRay (camera.GetEye (), field.GetFixSample (sampleRes, s) - camera.GetEye ());
				cl_rays.push_back (cameraRay);
			}
		}
	}

	std::vector<Color> pixmap;
	if (!RenderOnTheGPU (cl_rays, pixmap)) {
		return false;
	}

	int pix = 0;
	for (int y = rect.y; y < rect.y + rect.height; ++y) {
		for (int x = rect.x; x < rect.x + rect.width; ++x) {

			Average<Color> averageColor;
			for (int i = 0; i < sampleRes * sampleRes; i++) {
				averageColor.Add (pixmap[(pix * sampleRes * sampleRes) + i]);
			}

			const Color pixel = averageColor.Get ();
			result.SetColor (x, y, pixel);
			progressReport.Report (x, y, pixel.r, pixel.g, pixel.b, resX, resY);

			pix++;
		}
	}

	return true;
}

bool GPUTracer::RenderOnTheGPU (const std::vector<CL_Ray>& rays, std::vector<Color>& result) const
{
	if (DBGERROR (!initialized)) {
		return false;
	}

	cl_int error = 0;

	// Create ray and result buffers on the device.
	cl_mem ray_d = clCreateBuffer (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, rays.size () * sizeof (CL_Ray), (void *)&rays[0], &error);
	if (error != CL_SUCCESS) return false;
	cl_mem result_d = clCreateBuffer (context, CL_MEM_WRITE_ONLY, rays.size () * sizeof (CL_Vec4), nullptr, &error);
	if (error != CL_SUCCESS) return false;

	cl_int ray_count = rays.size ();

	// Set kernel arguments.
	error |= clSetKernelArg (kernel, 0, sizeof (cl_mem), &ray_d);
	error |= clSetKernelArg (kernel, 1, sizeof (cl_int), &ray_count);
	error |= clSetKernelArg (kernel, 6, sizeof (cl_mem), &result_d);

	// Run the ray trace and read back the result.
	//const size_t ws = 256;
	const size_t gs = rays.size ();
	error |= clEnqueueNDRangeKernel (command_queue, kernel, 1, nullptr, &gs, nullptr /* &ws */, 0, nullptr, nullptr);
	error |= clFinish (command_queue);

	std::vector<CL_Vec4> color_result;
	color_result.resize (rays.size ());
	error |= clEnqueueReadBuffer (command_queue, result_d, CL_TRUE, 0, rays.size () * sizeof (CL_Vec4), &color_result[0], 0, nullptr, nullptr);
	
	// Clean up.
	error |= clReleaseMemObject (ray_d);
	error |= clReleaseMemObject (result_d);

	if (error != CL_SUCCESS)
		return false;

	// Convert the result to the format used on the host.
	result.clear ();
	result.reserve (rays.size ());
	for (size_t i = 0; i < color_result.size (); ++i) {
		const CL_Vec4& c = color_result[i];
		result.push_back (Color (c.x, c.y, c.z));
	}

	return true;
}

GPUTracer::~GPUTracer ()
{
	if (initialized) {
		clReleaseKernel (kernel);
		clReleaseCommandQueue (command_queue);
		clReleaseContext (context);

		clReleaseMemObject (serializedGeometry_device);
		clReleaseMemObject (serializedLights_device);
	}
}

Color GPUTracer::GetFieldColor (const Image::Field& /*field*/) const
{
	throw "Don't peek while the gremlins are painting!";
}

#endif