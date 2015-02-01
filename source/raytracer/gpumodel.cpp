#include "gpumodel.hpp"

CL_Ray::CL_Ray (const Vec3& orig, const Vec3& dir) {
	origin.x = (cl_float)orig.x;
	origin.y = (cl_float)orig.y;
	origin.z = (cl_float)orig.z;

	direction.x = (cl_float)dir.x;
	direction.y = (cl_float)dir.y;
	direction.z = (cl_float)dir.z;
}

CL_Material::CL_Material (const CL_Vec4& color, double ambient, double diffuse, double specular, double shininess, double reflection, double transparency, double refractionIndex) :
	color (color),
	ambient ((cl_float) ambient),
	diffuse ((cl_float) diffuse),
	specular ((cl_float) specular),
	shininess ((cl_float) shininess),
	reflection ((cl_float) reflection),
	transparency ((cl_float) transparency),
	refractionIndex ((cl_float) refractionIndex)
{
}
