﻿
#ifndef __EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__
#define __EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__

#ifndef __DISABLED_DEFAULT_TEXTURE_LOADER__

#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <vector>

namespace EffekseerRenderer
{

class PngTextureLoader
{
private:
	std::vector<uint8_t> textureData;
	int32_t textureWidth;
	int32_t textureHeight;

public:
	bool Load(const void* data, int32_t size, bool rev);
	void Unload();

	std::vector<uint8_t>& GetData()
	{
		return textureData;
	}
	int32_t GetWidth()
	{
		return textureWidth;
	}
	int32_t GetHeight()
	{
		return textureHeight;
	}
};

} // namespace EffekseerRenderer

#endif

#endif
