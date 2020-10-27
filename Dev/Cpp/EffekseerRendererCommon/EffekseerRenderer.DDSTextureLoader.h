#ifndef __EFFEKSEERRENDERER_DDS_TEXTURE_LOADER_H__
#define __EFFEKSEERRENDERER_DDS_TEXTURE_LOADER_H__

#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <vector>

namespace EffekseerRenderer
{
class DDSTextureLoader
{
private:
	std::vector<uint8_t> textureData;
	int32_t textureWidth = 0;
	int32_t textureHeight = 0;
	Effekseer::TextureFormatType textureFormatType = Effekseer::TextureFormatType::ABGR8;

public:
	bool Load(void* data, int32_t size);
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
	Effekseer::TextureFormatType GetTextureFormat()
	{
		return textureFormatType;
	}
};
} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_DDS_TEXTURE_LOADER_H__