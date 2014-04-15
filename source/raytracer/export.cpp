#include "export.hpp"

#include <windows.h>
#include <gdiplus.h>

static bool GetEncoderClassId (const wchar_t* format, CLSID* classId)
{
	UINT count = 0;
	UINT size = 0;
	Gdiplus::Status status = Gdiplus::GetImageEncodersSize (&count, &size);
	if(status != Gdiplus::Ok || size == 0) {
		return false;
	}

	Gdiplus::ImageCodecInfo* imageCodecInfo = (Gdiplus::ImageCodecInfo*) (malloc(size));
	if(imageCodecInfo == NULL) {
		return false;
	}

	status = GetImageEncoders (count, size, imageCodecInfo);
	if(status != Gdiplus::Ok) {
		return false;
	}

	for (UINT i = 0; i < count; i++) {
		if (wcscmp (imageCodecInfo[i].MimeType, format) == 0) {
			*classId = imageCodecInfo[i].Clsid;
			free (imageCodecInfo);
			return true;
		}
	}

	free (imageCodecInfo);
	return false;
}

bool Export::ExportImage (const RayTracer::ResultImage& image, const std::wstring& fileName, const wchar_t* mimeType)
{
	Gdiplus::GdiplusStartupInput	input;
	Gdiplus::GdiplusStartupOutput	output;
	ULONG_PTR						gdiPlusToken;

	Gdiplus::Status status = Gdiplus::GdiplusStartup (&gdiPlusToken, &input, &output);
	if (status != Gdiplus::Ok) {
		return false;
	}

	{
		Gdiplus::Bitmap bitmap (image.GetResolutionX (), image.GetResolutionY ());
		for (int i = 0; i < image.GetResolutionX (); i++) {
			for (int j = 0; j < image.GetResolutionY (); j++) {
				Color color = image.GetColor (i, j);
				Gdiplus::Color gdiColor (BYTE (color.r * 255), BYTE (color.g * 255), BYTE (color.b * 255));
				bitmap.SetPixel (i, image.GetResolutionY () - j - 1, gdiColor);
			}
		}

		CLSID classId;
		if (!GetEncoderClassId (mimeType, &classId)) {
			return false;
		}

		status = bitmap.Save (fileName.c_str (), &classId);
		if (status != Gdiplus::Ok) {
			return false;
		}
	}

	Gdiplus::GdiplusShutdown (gdiPlusToken);
	return true;
}
