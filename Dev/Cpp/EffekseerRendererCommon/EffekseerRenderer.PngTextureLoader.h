
#ifndef	__EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__
#define	__EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__

#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <vector>

#if _WIN32
#define __PNG_DDI 1
#endif

#if _WIN32
#define WINVER          0x0501
#define _WIN32_WINNT    0x0501
#include <windows.h>
#include <gdiplus.h>
#endif

namespace EffekseerRenderer
{

class PngTextureLoader
{
private:

#ifdef __PNG_DDI
	Gdiplus::GdiplusStartupInput		gdiplusStartupInput;
	ULONG_PTR						gdiplusToken;
#endif

	std::vector<uint8_t> textureData;
	int32_t textureWidth;
	int32_t textureHeight;
public:

	bool Load(void* data, int32_t size, bool rev);
	void Unload();

	void Initialize();
	void Finalize();

	std::vector<uint8_t>& GetData() { return textureData; }
	int32_t GetWidth() { return textureWidth; }
	int32_t GetHeight() { return textureHeight; }
};

}

#endif
