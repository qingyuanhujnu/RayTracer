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

	double	focalLength;
	double	apertureRadius;

	if (!ReadNamedVec3 (inputStream, L"eye", eye)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"center", center)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"up", up)) { return false; }
	if (!ReadNamedDouble (inputStream, L"xfov", xFov)) { return false; }
	if (!ReadNamedDouble (inputStream, L"yfov", yFov)) { return false; }
	if (!ReadNamedDouble (inputStream, L"focal_length", focalLength)) { return false; }
	if (!ReadNamedDouble (inputStream, L"aperture_radius", apertureRadius)) { return false; }

	camera.Set (eye, center, up, xFov * DEGRAD, yFov * DEGRAD, focalLength, apertureRadius);
	return true;
}

static bool ReadLight (std::wifstream& inputStream, Model& model)
{
	Light::Shape shape = Light::SphereShape;
	Vec3 position;
	Color color;
	double radius = 0.0;
	double xSize = 0.0;
	double ySize = 0.0;
	double zSize = 0.0;
	Vec3 attenuation;

	std::wstring shapeString;
	if (!ReadNamedString (inputStream, L"shape", shapeString)) { return false; }
	if (shapeString == L"sphere") {
		shape = Light::SphereShape;
		if (!ReadDouble (inputStream, radius)) { return false; }
	} else if (shapeString == L"box") {
		shape = Light::BoxShape;
		if (!ReadDouble (inputStream, xSize)) { return false; }
		if (!ReadDouble (inputStream, ySize)) { return false; }
		if (!ReadDouble (inputStream, zSize)) { return false; }
	} else {
		return false;
	}

	if (!ReadNamedVec3 (inputStream, L"position", position)) { return false; }
	if (!ReadNamedColor (inputStream, L"color", color)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"attenuation", attenuation)) { return false; }

	Light light;
	if (shape == Light::SphereShape) {
		Sphere sphere (position, radius);
		light.Set (sphere, color, attenuation);
	} else if (shape == Light::BoxShape) {
		Box box (position, xSize, ySize, zSize);
		light.Set (box, color, attenuation);
	} else {
		return false;
	}
	model.AddLight (light);
	return true;
}

static bool ReadTexture (std::wifstream& inputStream, std::shared_ptr<Texture>& outTexture)
{
	std::wstring type;

	if (!ReadNamedString (inputStream, L"texture", type)) { return false; }
	if (type == L"mono") {
		Color color;
		if (!ReadNamedColor (inputStream, L"color", color)) { return false; }
		outTexture.reset (new MonoTexture (color));
		return true;
	}
	else if (type == L"checkerboard") {
		int horiz = 0;
		int vert = 0;
		if (!ReadNamedInteger (inputStream, L"horiz", horiz)) { return false; }
		if (!ReadNamedInteger (inputStream, L"vert", vert)) { return false; }
		outTexture.reset (new CheckBoardTexture (horiz, vert));
		return true;
	}
	else if (type == L"image") {
		std::wstring texture;
		if (!ReadNamedString (inputStream, L"texture", texture)) { return false; }
		// TODO 
		return false;
	}
	else {
		return false;
	}
}

static bool ReadMaterial (std::wifstream& inputStream, Model& model)
{
	std::shared_ptr<Texture> texture;
	double ambient;
	double diffuse;
	double specular;
	double shininess;
	double reflection;
	double transparency;
	double refractionIndex;

	if (!ReadTexture (inputStream, texture)) { return false; }
	if (!ReadNamedDouble (inputStream, L"ambient", ambient)) { return false; }
	if (!ReadNamedDouble (inputStream, L"diffuse", diffuse)) { return false; }
	if (!ReadNamedDouble (inputStream, L"specular", specular)) { return false; }
	if (!ReadNamedDouble (inputStream, L"shininess", shininess)) { return false; }
	if (!ReadNamedDouble (inputStream, L"reflection", reflection)) { return false; }
	if (!ReadNamedDouble (inputStream, L"transparency", transparency)) { return false; }
	if (!ReadNamedDouble (inputStream, L"refraction_index", refractionIndex)) { return false; }

	Material material (texture, ambient, diffuse, specular, shininess, reflection, transparency, refractionIndex);
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

static bool ReadCuboid (std::wifstream& inputStream, Model& model)
{
	Vec3 size;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedVec3 (inputStream, L"size", size)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::GenerateCuboid (model, size.x, size.y, size.z, offset, rotation * DEGRAD, material);
	return true;
}

static bool ReadRoomBox (std::wifstream& inputStream, Model& model)
{
	Vec3 size;
	Vec3 offset;
	Vec3 rotation;

	if (!ReadNamedVec3 (inputStream, L"size", size)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }

	std::wstring command;
	if (!ReadString (inputStream, command) || command != L"materials") {
		return false;
	}

	UIndex materials[6];
	if (!ReadUIndex (inputStream, materials[0])) { return false; }
	if (!ReadUIndex (inputStream, materials[1])) { return false; }
	if (!ReadUIndex (inputStream, materials[2])) { return false; }
	if (!ReadUIndex (inputStream, materials[3])) { return false; }
	if (!ReadUIndex (inputStream, materials[4])) { return false; }
	if (!ReadUIndex (inputStream, materials[5])) { return false; }

	Generator::GenerateRoomBox (model, size.x, size.y, size.z, offset, rotation * DEGRAD, materials);
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

static bool ReadTorus (std::wifstream& inputStream, Model& model)
{
	double outerRadius;
	double innerRadius;
	int outerSegmentation;
	int innerSegmentation;
	Vec3 offset;
	Vec3 rotation;
	UIndex material;

	if (!ReadNamedDouble (inputStream, L"outerradius", outerRadius)) { return false; }
	if (!ReadNamedDouble (inputStream, L"innerradius", innerRadius)) { return false; }
	if (!ReadNamedInteger (inputStream, L"outersegmentation", outerSegmentation)) { return false; }
	if (!ReadNamedInteger (inputStream, L"innersegmentation", innerSegmentation)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }
	if (!ReadNamedUIndex (inputStream, L"material", material)) { return false; }
	
	Generator::GenerateTorus (model, outerRadius, innerRadius, outerSegmentation, innerSegmentation, offset, rotation * DEGRAD, material);
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

static bool ReadMesh (std::wifstream& inputStream, Model& model)
{
	std::wstring command;
	if (!ReadString (inputStream, command) || command != L"vertices") {
		return false;
	}

	UIndex vertexCount = 0;
	if (!ReadUIndex (inputStream, vertexCount)) {
		return false;
	}

	std::vector<Vec3> vertices;
	for (UIndex i = 0; i < vertexCount; i++) {
		Vec3 vertex;
		if (!ReadVec3 (inputStream, vertex)) {
			return false;
		}
		vertices.push_back (vertex);
	}

	if (!ReadString (inputStream, command) || command != L"normals") {
		return false;
	}

	UIndex normalCount = 0;
	if (!ReadUIndex (inputStream, normalCount)) {
		return false;
	}

	std::vector<Vec3> normals;
	for (UIndex i = 0; i < normalCount; i++) {
		Vec3 normal;
		if (!ReadVec3 (inputStream, normal)) {
			return false;
		}
		normals.push_back (normal);
	}

	if (!ReadString (inputStream, command) || command != L"triangles") {
		return false;
	}

	UIndex triangleCount = 0;
	if (!ReadUIndex (inputStream, triangleCount)) {
		return false;
	}

	std::vector<Mesh::Triangle> triangles;
	for (UIndex i = 0; i < triangleCount; i++) {
		UIndex v0;
		UIndex v1;
		UIndex v2;
		UIndex n0;
		UIndex n1;
		UIndex n2;
		UIndex curveGroup;
		UIndex material;
		if (!ReadUIndex (inputStream, v0)) { return false; }
		if (!ReadUIndex (inputStream, v1)) { return false; }
		if (!ReadUIndex (inputStream, v2)) { return false; }
		if (!ReadUIndex (inputStream, n0)) { return false; }
		if (!ReadUIndex (inputStream, n1)) { return false; }
		if (!ReadUIndex (inputStream, n2)) { return false; }
		if (!ReadUIndex (inputStream, material)) { return false; }
		if (!ReadUIndex (inputStream, curveGroup)) { return false; }
		if (n0 == InvalidIndex || n1 == InvalidIndex || n2 == InvalidIndex) {
			triangles.push_back (Mesh::Triangle (v0, v1, v2, material, curveGroup));
		} else {
			triangles.push_back (Mesh::Triangle (v0, v1, v2, n0, n1, n2, material));
		}
	}

	Vec3 offset;
	Vec3 rotation;
	if (!ReadNamedVec3 (inputStream, L"offset", offset)) { return false; }
	if (!ReadNamedVec3 (inputStream, L"rotation", rotation)) { return false; }

	Generator::GenerateMesh (model, vertices, normals, triangles, offset, rotation * DEGRAD);
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
			if (DBGERROR (!ReadCuboid (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"roombox") {
			if (DBGERROR (!ReadRoomBox (inputStream, model))) {
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
		} else if (commandName == L"torus") {
			if (DBGERROR (!ReadTorus (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"solid") {
			if (DBGERROR (!ReadSolid (inputStream, model))) {
				error = true;
			}
		} else if (commandName == L"mesh") {
			if (DBGERROR (!ReadMesh (inputStream, model))) {
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
