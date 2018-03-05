
#pragma once

#include <Effekseer.h>
#include <vector>

#define NONDLL	1

#ifdef _WIN32
#define MSWIN32 1
#define BGDWIN32 1
#endif
#include <gd/gd.h>
#include <gd/gdfontmb.h>

#ifdef _WIN32
#if _DEBUG
#pragma comment(lib,"x86/Debug/libgd_static.lib")
#else
#pragma comment(lib,"x86/Release/libgd_static.lib")
#endif
#endif

namespace efk
{
	class GifHelper
	{
	private:
		FILE*		fp = nullptr;
		gdImagePtr	img = nullptr;
		int32_t		width = 0;
		int32_t		height = 0;
		int32_t		freq = 0;

	public:

		GifHelper() = default;

		virtual ~GifHelper();

		bool Initialize(const char16_t* path, int32_t width, int32_t height, int32_t freq);

		void AddImage(const std::vector<Effekseer::Color>& pixels);
	};

}