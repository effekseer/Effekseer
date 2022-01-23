
#pragma once

#include <Effekseer.h>
#include <stdint.h>
#include <vector>

#ifdef _WIN32

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#include <windows.h>

#endif

#define Z_SOLO
#include <png.h>
//#include <pngstruct.h>
//#include <pnginfo.h>

namespace efk
{
class PNGHelper
{
public:
	PNGHelper();

	~PNGHelper();

	bool Save(const char16_t* path, int32_t width, int32_t height, const void* data);
};
} // namespace efk