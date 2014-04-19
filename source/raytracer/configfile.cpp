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

static bool ReadNamedString (std::wifstream& inputStream, const std::wstring& name, std::wstring& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadString (inputStream, val)) { return false; }
	return true;
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

static bool ReadVec2 (std::wifstream& inputStream, Vec3& val)
{
	if (!ReadDouble (inputStream, val.x)) { return false; }
	if (!ReadDouble (inputStream, val.y)) { return false; }
	val.z = 0.0;
	return true;
}

static bool ReadNamedVec2 (std::wifstream& inputStream, const std::wstring& name, Vec3& val)
{
	std::wstring readName;
	if (!ReadString (inputStream, readName) || readName != name) { return false; }
	if (!ReadVec2 (inputStream, val)) { return false; }
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

static bool ReadParameters (std::wifstream& inputStream, Renderer::Parameters& parameters)
{
	int resolutionX;
	int resolutionY;
	double imageDistance;

	if (!ReadNamedInteger (inputStream, L"xresolution", resolutionX)) { return false; }
	if (!ReadNamedInteger (inputStream, L"yresolution", resolutionY)) { return false; }
	if (!ReadNamedDouble (inputStream, L"imagedistance", imageDistance)) { return false; }
	
	parameters.Set (resolutionX, resolutionY, imageDistance);
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

	camera.Set (eye, center, up, xFov * DEGRAD, yFov * DEGRAD);
	return true;
}

static bool ReadLight (std::wifstream& inputStream, Model& model)
{
	Vec3 position;
	Color color;

	if (!ReadNamedVec3 (inputStream, L"position", position)) { return false; }
	if (!ReadNamedColor (inputStream, L"color", color)) { return false; }

	Light light;
	light.Set (position, color);
	model.AddLight (light);
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

static bool ReadRectangle (std::wifstream& inputStream, Model& model)
{
	Vec3 size;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedVec2 (inputStream, L"size", size)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::GenerateRectangle (model, size.x, size.y, offset, rotation * DEGRAD, material);
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

static bool ReadSolid (std::wifstream& inputStream, Model& model)
{
	std::wstring type;
	double radius;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedString (inputStream, L"type", type)) { return false; }
	if (!ReadNamedDouble (inputStream, L"radius", radius)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::SolidType solidType;
	if (type == L"tetrahedron") {
		solidType = Generator::Tetrahedron;
	} else if (type == L"hexahedron") {
		solidType = Generator::Hexahedron;
	} else if (type == L"octahedron") {
		solidType = Generator::Octahedron;
	} else if (type == L"dodecahedron") {
		solidType = Generator::Dodecahedron;
	} else if (type == L"icosahedron") {
		solidType = Generator::Icosahedron;
	} else {
		DBGERROR (true);
		return false;
	}
	Generator::GenerateSolid (model, solidType, radius, offset, rotation * DEGRAD, material);
	return true;
}

bool ConfigFile::Read (const std::wstring& fileName, Renderer::Parameters& parameters, Camera& camera, Model& model)
{
	std::wifstream inputStream (fileName.c_str ());
	if (DBGERROR (!inputStream)) {
		return false;
	}

	int version;
	if (!ReadNamedInteger (inputStream, L"version", version)) { return false; }

	bool error = false;
	std::wstring commandName;
	while (!error && ReadString (inputStream, commandName)) {
		if (commandName == L"parameters") {
			if (DBGERROR (!ReadParameters (inputStream, parameters))) {
				return false;
			}
		} else if (commandName == L"camera") {
			if (DBGERROR (!ReadCamera (inputStream, camera))) {
				error = true;
			}
		} else if (commandName == L"light") {
			if (DBGERROR (!ReadLight (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"material") {
			Material material;
			if (DBGERROR (!ReadMaterial (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"rectangle") {
			if (DBGERROR (!ReadRectangle (inputStream, model))) {
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
		} else if (commandName == L"solid") {
			if (DBGERROR (!ReadSolid (inputStream, model))) {
				error = true;
			}
		} else {
			DBGERROR (true);		// other fluff in the file
			error = true;
		}
	}

	inputStream.close ();
	return !error;
}
