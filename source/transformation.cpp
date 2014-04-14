#include "transformation.hpp"
#include "common.hpp"

#include <math.h>

Transformation::Transformation ()
{
	SetIdentity ();
}

Transformation::Transformation (const Transformation& source)
{
	source.Clone (*this);
}

Transformation::~Transformation ()
{

}

void Transformation::SetIdentity ()
{
	matrix[0] = 1.0;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = 1.0;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = 0.0;
	matrix[9] = 0.0;
	matrix[10] = 1.0;
	matrix[11] = 0.0;
	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = 0.0;
	matrix[15] = 1.0;
}

void Transformation::SetTranslation (const Vec3& translation)
{
	matrix[0] = 1.0;
	matrix[1] = 0.0;
	matrix[2] = 0.0;
	matrix[3] = 0.0;
	matrix[4] = 0.0;
	matrix[5] = 1.0;
	matrix[6] = 0.0;
	matrix[7] = 0.0;
	matrix[8] = 0.0;
	matrix[9] = 0.0;
	matrix[10] = 1.0;
	matrix[11] = 0.0;
	matrix[12] = translation.x;
	matrix[13] = translation.y;
	matrix[14] = translation.z;
	matrix[15] = 1.0;
}

void Transformation::SetRotationX (double angle)
{
	double si = sin (angle);
	double co = cos (angle);

    matrix[0] = 1.0;
    matrix[1] = 0.0;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = co;
    matrix[6] = -si;
    matrix[7] = 0.0;
    matrix[8] = 0.0;
    matrix[9] = si;
    matrix[10] = co;
    matrix[11] = 0.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = 0.0;
    matrix[15] = 1.0;
}

void Transformation::SetRotationY (double angle)
{
	double si = sin (angle);
	double co = cos (angle);

    matrix[0] = co;
    matrix[1] = 0.0;
    matrix[2] = si;
    matrix[3] = 0.0;
    matrix[4] = 0.0;
    matrix[5] = 1.0;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = -si;
    matrix[9] = 0.0;
    matrix[10] = co;
    matrix[11] = 0.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = 0.0;
    matrix[15] = 1.0;
}

void Transformation::SetRotationZ (double angle)
{
	double si = sin (angle);
	double co = cos (angle);

    matrix[0] = co;
    matrix[1] = -si;
    matrix[2] = 0.0;
    matrix[3] = 0.0;
    matrix[4] = si;
    matrix[5] = co;
    matrix[6] = 0.0;
    matrix[7] = 0.0;
    matrix[8] = 0.0;
    matrix[9] = 0.0;
    matrix[10] = 1.0;
    matrix[11] = 0.0;
    matrix[12] = 0.0;
    matrix[13] = 0.0;
    matrix[14] = 0.0;
    matrix[15] = 1.0;
}

void Transformation::Append (const Transformation& mat)
{
	double a00 = matrix[0];
	double a01 = matrix[1];
	double a02 = matrix[2];
	double a03 = matrix[3];
	double a10 = matrix[4];
	double a11 = matrix[5];
	double a12 = matrix[6];
	double a13 = matrix[7];
	double a20 = matrix[8];
	double a21 = matrix[9];
	double a22 = matrix[10];
	double a23 = matrix[11];
	double a30 = matrix[12];
	double a31 = matrix[13];
	double a32 = matrix[14];
	double a33 = matrix[15];

	double b00 = mat.matrix[0];
	double b01 = mat.matrix[1];
	double b02 = mat.matrix[2];
	double b03 = mat.matrix[3];
	double b10 = mat.matrix[4];
	double b11 = mat.matrix[5];
	double b12 = mat.matrix[6];
	double b13 = mat.matrix[7];
	double b20 = mat.matrix[8];
	double b21 = mat.matrix[9];
	double b22 = mat.matrix[10];
	double b23 = mat.matrix[11];
	double b30 = mat.matrix[12];
	double b31 = mat.matrix[13];
	double b32 = mat.matrix[14];
	double b33 = mat.matrix[15];
		
	matrix[0] = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
	matrix[1] = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
	matrix[2] = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
	matrix[3] = a00 * b03 + a01 * b13 + a02 * b23 + a03 * b33;
	matrix[4] = a10 * b00 + a11 * b10 + a12 * b20 + a13 * b30;
	matrix[5] = a10 * b01 + a11 * b11 + a12 * b21 + a13 * b31;
	matrix[6] = a10 * b02 + a11 * b12 + a12 * b22 + a13 * b32;
	matrix[7] = a10 * b03 + a11 * b13 + a12 * b23 + a13 * b33;
	matrix[8] = a20 * b00 + a21 * b10 + a22 * b20 + a23 * b30;
	matrix[9] = a20 * b01 + a21 * b11 + a22 * b21 + a23 * b31;
	matrix[10] = a20 * b02 + a21 * b12 + a22 * b22 + a23 * b32;
	matrix[11] = a20 * b03 + a21 * b13 + a22 * b23 + a23 * b33;
	matrix[12] = a30 * b00 + a31 * b10 + a32 * b20 + a33 * b30;
	matrix[13] = a30 * b01 + a31 * b11 + a32 * b21 + a33 * b31;
	matrix[14] = a30 * b02 + a31 * b12 + a32 * b22 + a33 * b32;
	matrix[15] = a30 * b03 + a31 * b13 + a32 * b23 + a33 * b33;
}

void Transformation::AppendTranslation (const Vec3& translation)
{
	Transformation transformation;
	transformation.SetTranslation (translation);
	Append (transformation);
}

void Transformation::AppendRotationX (double angle)
{
	Transformation transformation;
	transformation.SetRotationX (angle);
	Append (transformation);
}

void Transformation::AppendRotationY (double angle)
{
	Transformation transformation;
	transformation.SetRotationY (angle);
	Append (transformation);
}

void Transformation::AppendRotationZ (double angle)
{
	Transformation transformation;
	transformation.SetRotationZ (angle);
	Append (transformation);
}

void Transformation::AppendRotationXYZ (const Vec3& rotation)
{
	AppendRotationX (rotation.x);
	AppendRotationY (rotation.y);
	AppendRotationZ (rotation.z);
}

Vec3 Transformation::Apply (const Vec3& vec) const
{
	double a00 = vec.x;
	double a01 = vec.y;
	double a02 = vec.z;
	double a03 = 1.0;
	double b00 = matrix[0];
	double b01 = matrix[1];
	double b02 = matrix[2];
	double b10 = matrix[4];
	double b11 = matrix[5];
	double b12 = matrix[6];
	double b20 = matrix[8];
	double b21 = matrix[9];
	double b22 = matrix[10];
	double b30 = matrix[12];
	double b31 = matrix[13];
	double b32 = matrix[14];

	Vec3 result;
	result.x = a00 * b00 + a01 * b10 + a02 * b20 + a03 * b30;
	result.y = a00 * b01 + a01 * b11 + a02 * b21 + a03 * b31;
	result.z = a00 * b02 + a01 * b12 + a02 * b22 + a03 * b32;
	return result;
}

void Transformation::Clone (Transformation& mat) const
{
	mat.matrix[0] = matrix[0];
	mat.matrix[1] = matrix[1];
	mat.matrix[2] = matrix[2];
	mat.matrix[3] = matrix[3];
	mat.matrix[4] = matrix[4];
	mat.matrix[5] = matrix[5];
	mat.matrix[6] = matrix[6];
	mat.matrix[7] = matrix[7];
	mat.matrix[8] = matrix[8];
	mat.matrix[9] = matrix[9];
	mat.matrix[10] = matrix[10];
	mat.matrix[11] = matrix[11];
	mat.matrix[12] = matrix[12];
	mat.matrix[13] = matrix[13];
	mat.matrix[14] = matrix[14];
	mat.matrix[15] = matrix[15];
}
