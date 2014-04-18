#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <string>

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "renderer.hpp"

class ConfigFile
{
public:
	static bool Read (const std::wstring& fileName, Renderer::Parameters& parameters, Camera& camera, Model& model);
};

#endif
