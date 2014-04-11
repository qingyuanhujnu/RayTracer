#include "configfile.hpp"
#include "generator.hpp"

#include <fstream>

static bool ReadDouble (std::wifstream& inputStream, double& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadInteger (std::wifstream& inputStream, int& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadUIndex (std::wifstream& inputStream, UIndex& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadVec3 (std::wifstream& inputStream, Vec3& Vec3)
{
	if (!ReadDouble (inputStream, Vec3.x)) { return false; }
	if (!ReadDouble (inputStream, Vec3.y)) { return false; }
	if (!ReadDouble (inputStream, Vec3.z)) { return false; }
	return true;
}

static bool ReadColor (std::wifstream& inputStream, Color& color)
{
	if (!ReadDouble (inputStream, color.r)) { return false; }
	if (!ReadDouble (inputStream, color.g)) { return false; }
	if (!ReadDouble (inputStream, color.b)) { return false; }
	return true;
}

static bool ReadCamera (std::wifstream& inputStream, Camera& camera)
{
	Vec3	eye;
	Vec3	center;
	Vec3	up;

	double	xFov;
	double	yFov;

	if (!ReadVec3 (inputStream, eye)) { return false; }
	if (!ReadVec3 (inputStream, center)) { return false; }
	if (!ReadVec3 (inputStream, up)) { return false; }
	if (!ReadDouble (inputStream, xFov)) { return false; }
	if (!ReadDouble (inputStream, yFov)) { return false; }

	camera.Set (eye, center, up, xFov, yFov);
	return true;
}

static bool ReadLight (std::wifstream& inputStream, Light& light)
{
	Vec3 position;
	Color color;
	double ambient;
	double diffuse;
	double specular;

	if (!ReadVec3 (inputStream, position)) { return false; }
	if (!ReadColor (inputStream, color)) { return false; }
	if (!ReadDouble (inputStream, ambient)) { return false; }
	if (!ReadDouble (inputStream, diffuse)) { return false; }
	if (!ReadDouble (inputStream, specular)) { return false; }

	light.Set (position, color, ambient, diffuse, specular);
	return true;
}

static bool ReadMaterial (std::wifstream& inputStream, Model& model)
{
	Color color;
	double ambient;
	double diffuse;
	double specular;
	double reflection;

	if (!ReadColor (inputStream, color)) { return false; }
	if (!ReadDouble (inputStream, ambient)) { return false; }
	if (!ReadDouble (inputStream, diffuse)) { return false; }
	if (!ReadDouble (inputStream, specular)) { return false; }
	if (!ReadDouble (inputStream, reflection)) { return false; }

	Material material;
	material.Set (color, ambient, diffuse, specular, reflection);
	model.AddMaterial (material);
	return true;
}

static bool ReadCuboid (std::wifstream& inputStream, Model& model, Generator::Facing facing)
{
	double xSize;
	double ySize;
	double zSize;
	Vec3 offset;
	UIndex material;

	if (!ReadDouble (inputStream, xSize)) { return false; }
	if (!ReadDouble (inputStream, ySize)) { return false; }
	if (!ReadDouble (inputStream, zSize)) { return false; }
	if (!ReadVec3 (inputStream, offset)) { return false; }
	if (!ReadUIndex (inputStream, material)) { return false; }
	
	if (facing == Generator::Inside) {
		Generator::GenerateCuboid (model, xSize, ySize, zSize, offset, material);
	} else if (facing == Generator::Outside) {
		Generator::GenerateInsideOutCuboid (model, xSize, ySize, zSize, offset, material);
	} else {
		DBGERROR (true);
	}
	return true;
}

static bool ReadCylinder (std::wifstream& inputStream, Model& model)
{
	double radius;
	double height;
	int segmentation;
	Vec3 offset;
	UIndex material;

	if (!ReadDouble (inputStream, radius)) { return false; }
	if (!ReadDouble (inputStream, height)) { return false; }
	if (!ReadInteger (inputStream, segmentation)) { return false; }
	if (!ReadVec3 (inputStream, offset)) { return false; }
	if (!ReadUIndex (inputStream, material)) { return false; }
	
	Generator::GenerateCylinder (model, radius, height, segmentation, offset, material);
	return true;
}

static bool ReadSphere (std::wifstream& inputStream, Model& model)
{
	double radius;
	int segmentation;
	Vec3 offset;
	UIndex material;

	if (!ReadDouble (inputStream, radius)) { return false; }
	if (!ReadInteger (inputStream, segmentation)) { return false; }
	if (!ReadVec3 (inputStream, offset)) { return false; }
	if (!ReadUIndex (inputStream, material)) { return false; }
	
	Generator::GenerateSphere (model, radius, segmentation, offset, material);
	return true;
}

bool ConfigFile::Read (const std::wstring& fileName, Camera& camera, Light& light, Model& model)
{
	std::wifstream inputStream (fileName.c_str ());
	if (DBGERROR (!inputStream)) {
		return false;
	}

	bool error = false;
	std::wstring commandName;
	while (!error && inputStream >> commandName) {
		if (commandName == L"camera") {
			if (DBGERROR (!ReadCamera (inputStream, camera))) {
				error = true;
			}
		} else if (commandName == L"light") {
			if (DBGERROR (!ReadLight (inputStream, light))) {
				error = true;
			}
		} else if (commandName == L"material") {
			Material material;
			if (DBGERROR (!ReadMaterial (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"cuboid") {
			if (DBGERROR (!ReadCuboid (inputStream, model, Generator::Inside))) {
				error = true;
			}
		} else if (commandName == L"insideoutcuboid") {
			if (DBGERROR (!ReadCuboid (inputStream, model, Generator::Outside))) {
				error = true;
			}
		} else if (commandName == L"cylinder") {
			if (DBGERROR (!ReadCylinder (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"sphere") {
			if (DBGERROR (!ReadSphere (inputStream, model))) {
				error = true;
			}
		} else {
			error = true;
		}
	}

	inputStream.close ();
	return !error;
}
