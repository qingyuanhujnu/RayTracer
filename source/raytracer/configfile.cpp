#include "configfile.hpp"
#include "generator.hpp"

#include <fstream>

static bool ReadString (std::wifstream& inputStream, std::wstring& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadDouble (std::wifstream& inputStream, double& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadNamedDouble (std::wifstream& inputStream, const std::wstring& name, double& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadDouble (inputStream, val)) { return false; }
	return true;
}

static bool ReadInteger (std::wifstream& inputStream, int& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadNamedInteger (std::wifstream& inputStream, const std::wstring& name, int& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadInteger (inputStream, val)) { return false; }
	return true;
}

static bool ReadUIndex (std::wifstream& inputStream, UIndex& val)
{
	if (inputStream >> val) {
		return true;
	}
	return false;
}

static bool ReadNamedUIndex (std::wifstream& inputStream, const std::wstring& name, UIndex& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadUIndex (inputStream, val)) { return false; }
	return true;
}

static bool ReadVec3 (std::wifstream& inputStream, Vec3& val)
{
	if (!ReadDouble (inputStream, val.x)) { return false; }
	if (!ReadDouble (inputStream, val.y)) { return false; }
	if (!ReadDouble (inputStream, val.z)) { return false; }
	return true;
}

static bool ReadNamedVec3 (std::wifstream& inputStream, const std::wstring& name, Vec3& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadVec3 (inputStream, val)) { return false; }
	return true;
}

static bool ReadColor (std::wifstream& inputStream, Color& color)
{
	if (!ReadDouble (inputStream, color.r)) { return false; }
	if (!ReadDouble (inputStream, color.g)) { return false; }
	if (!ReadDouble (inputStream, color.b)) { return false; }
	return true;
}

static bool ReadNamedColor (std::wifstream& inputStream, const std::wstring& name, Color& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadColor (inputStream, val)) { return false; }
	return true;
}

static bool ReadCamera (std::wifstream& inputStream, Camera& camera)
{
	Vec3	eye;
	Vec3	center;
	Vec3	up;

	double	xFov;
	double	yFov;

	if (!ReadNamedVec3 (inputStream, L"eye", eye)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"center", center)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"up", up)) { return false; }
	if (!ReadNamedDouble (inputStream, L"xfov", xFov)) { return false; }
	if (!ReadNamedDouble (inputStream, L"yfov", yFov)) { return false; }

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

	if (!ReadNamedVec3 (inputStream, L"position", position)) { return false; }
	if (!ReadNamedColor (inputStream, L"color", color)) { return false; }
	if (!ReadNamedDouble (inputStream, L"ambient", ambient)) { return false; }
	if (!ReadNamedDouble (inputStream, L"diffuse", diffuse)) { return false; }
	if (!ReadNamedDouble (inputStream, L"specular", specular)) { return false; }

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

	if (!ReadNamedColor (inputStream, L"color", color)) { return false; }
	if (!ReadNamedDouble (inputStream, L"ambient", ambient)) { return false; }
	if (!ReadNamedDouble (inputStream, L"diffuse", diffuse)) { return false; }
	if (!ReadNamedDouble (inputStream, L"specular", specular)) { return false; }
	if (!ReadNamedDouble (inputStream, L"reflection", reflection)) { return false; }

	Material material;
	material.Set (color, ambient, diffuse, specular, reflection);
	model.AddMaterial (material);
	return true;
}

static bool ReadCuboid (std::wifstream& inputStream, Model& model, Generator::Facing facing)
{
	Vec3 size;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedVec3 (inputStream, L"size", size)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	if (facing == Generator::Inside) {
		Generator::GenerateCuboid (model, size.x, size.y, size.z, offset, rotation * DEGRAD, material);
	} else if (facing == Generator::Outside) {
		Generator::GenerateInsideOutCuboid (model, size.x, size.y, size.z, offset, rotation * DEGRAD, material);
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
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedDouble (inputStream, L"radius", radius)) { return false; }
	if (!ReadNamedDouble (inputStream, L"height", height)) { return false; }
	if (!ReadNamedInteger (inputStream, L"segmentation", segmentation)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::GenerateCylinder (model, radius, height, segmentation, offset, rotation * DEGRAD, material);
	return true;
}

static bool ReadSphere (std::wifstream& inputStream, Model& model)
{
	double radius;
	int segmentation;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedDouble (inputStream, L"radius", radius)) { return false; }
	if (!ReadNamedInteger (inputStream, L"segmentation", segmentation)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::GenerateSphere (model, radius, segmentation, offset, rotation * DEGRAD, material);
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
	while (!error && ReadString (inputStream, commandName)) {
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
