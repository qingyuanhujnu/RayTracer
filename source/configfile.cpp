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

static bool ReadUIndex (std::wifstream& inputStream, UIndex& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadCoord (std::wifstream& inputStream, Coord& coord)
{
	if (!ReadDouble (inputStream, coord.x)) { return false; }
	if (!ReadDouble (inputStream, coord.y)) { return false; }
	if (!ReadDouble (inputStream, coord.z)) { return false; }
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
	Coord	eye;
	Coord	center;
	Coord	up;

	double	xFov;
	double	yFov;

	if (!ReadCoord (inputStream, eye)) { return false; }
	if (!ReadCoord (inputStream, center)) { return false; }
	if (!ReadCoord (inputStream, up)) { return false; }
	if (!ReadDouble (inputStream, xFov)) { return false; }
	if (!ReadDouble (inputStream, yFov)) { return false; }

	camera.Set (eye, center, up, xFov, yFov);
	return true;
}

static bool ReadLight (std::wifstream& inputStream, Light& light)
{
	Coord position;
	Color color;
	double ambient;
	double diffuse;
	double specular;

	if (!ReadCoord (inputStream, position)) { return false; }
	if (!ReadColor (inputStream, color)) { return false; }
	if (!ReadDouble (inputStream, ambient)) { return false; }
	if (!ReadDouble (inputStream, diffuse)) { return false; }
	if (!ReadDouble (inputStream, specular)) { return false; }

	light.Set (position, color, ambient, diffuse, specular);
	return true;
}

static bool ReadMaterial (std::wifstream& inputStream, Material& material)
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

	material.Set (color, ambient, diffuse, specular, reflection);
	return true;
}

static bool ReadCuboid (std::wifstream& inputStream, Model& model, bool inverse)
{
	double xSize;
	double ySize;
	double zSize;
	Coord offset;
	UIndex material;

	if (!ReadDouble (inputStream, xSize)) { return false; }
	if (!ReadDouble (inputStream, ySize)) { return false; }
	if (!ReadDouble (inputStream, zSize)) { return false; }
	if (!ReadCoord (inputStream, offset)) { return false; }
	if (!ReadUIndex (inputStream, material)) { return false; }
	
	if (inverse) {
		Generator::GenerateInverseCuboid (model, xSize, ySize, zSize, offset, material);
	} else {
		Generator::GenerateCuboid (model, xSize, ySize, zSize, offset, material);
	}
	return true;
}

bool ConfigFile::Read (const std::wstring& fileName, Camera& camera, Light& light, Model& model)
{
	std::wifstream inputStream (fileName.c_str ());
	if (!inputStream) {
		return false;
	}

	std::wstring commandName;
	while (inputStream >> commandName) {
		if (commandName == L"camera") {
			if (!ReadCamera (inputStream, camera)) {
				return false;
			}
		} else if (commandName == L"light") {
			if (!ReadLight (inputStream, light)) {
				return false;
			}
		} else if (commandName == L"material") {
			Material material;
			if (!ReadMaterial (inputStream, material)) {
				return false;
			}
			model.AddMaterial (material);
		} else if (commandName == L"cuboid") {
			ReadCuboid (inputStream, model, false);
		} else if (commandName == L"inversecuboid") {
			ReadCuboid (inputStream, model, true);
		}
	}

	inputStream.close ();
	return true;
}
