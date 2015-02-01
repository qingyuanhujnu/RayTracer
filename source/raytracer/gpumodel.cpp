#include "gpumodel.hpp"

CL_Ray::CL_Ray (const Vec3& orig, const Vec3& dir) {
	origin.x = (cl_float)orig.x;
	origin.y = (cl_float)orig.y;
	origin.z = (cl_float)orig.z;

	direction.x = (cl_float)dir.x;
	direction.y = (cl_float)dir.y;
	direction.z = (cl_float)dir.z;
}