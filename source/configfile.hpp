#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <string>

#include "camera.hpp"
#include "light.hpp"
#include "mesh.hpp"

class ConfigFile
{
public:
	static bool Read (const std::wstring& fileName, Camera& camera, Light& light, Mesh& mesh);
};

#endif
