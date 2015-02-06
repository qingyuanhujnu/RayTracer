#ifndef GPUMODEL_HPP
#define GPUMODEL_HPP

#define OPENCL
#ifdef OPENCL

#include "CL/cl.h"
#include "vec3.hpp"

struct CL_Vec4 {
	cl_float x, y, z, w;

	CL_Vec4 () : x (0.f), y (0.f), z (0.f), w (0.0f) {}
	CL_Vec4 (float x, float y, float z) : x (x), y (y), z (z), w (0.0f) {}
	explicit CL_Vec4 (const Vec3& v) : x ((float)v.x), y ((float)v.y), z ((float) v.z), w (0.0f) {}
};

struct CL_Ray {
	CL_Vec4 origin;
	CL_Vec4 direction;

	CL_Ray (const Vec3& orig, const Vec3& dir);
};

struct CL_Triangle {
	CL_Vec4 a, b, c;	// vertices
	CL_Vec4 na, nb, nc;		// normals
	cl_int matIdx;
	cl_int doubleSided;
	cl_int filler[2];	// 16 byte alignment
};

struct CL_Light {
	CL_Vec4 pos;
	CL_Vec4 color;
	CL_Vec4 attenuation;
};

struct CL_Material {
	CL_Vec4 color;

	cl_float ambient;
	cl_float diffuse;
	cl_float specular;
	cl_float shininess;
	
	cl_float reflection;
	cl_float transparency;
	cl_float refractionIndex;

	cl_int filler;	// 8 byte alignment

	CL_Material (const CL_Vec4& color, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex);
};


#endif

#endif