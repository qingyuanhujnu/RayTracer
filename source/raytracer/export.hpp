#ifndef EXPORT_HPP
#define EXPORT_HPP

#include "raytracer.hpp"

class Export
{
public:
	static bool ExportImage (const RayTracer::ResultImage& image, const std::wstring& fileName, const wchar_t* mimeType);
};

#endif
