#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <string>

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"

class ConfigFile
{
public:
	static bool Read (const std::wstring& fileName, Camera& camera, Light& light, Model& model);
};

#endif
