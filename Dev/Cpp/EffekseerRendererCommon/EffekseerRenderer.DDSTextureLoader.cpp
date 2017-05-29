#include "EffekseerRenderer.DDSTextureLoader.h"

namespace EffekseerRenderer
{
	std::vector<uint8_t> DDSTextureLoader::textureData;
	int32_t DDSTextureLoader::textureWidth = 0;
	int32_t DDSTextureLoader::textureHeight = 0;
	Effekseer::TextureFormatType	DDSTextureLoader::textureFormatType = Effekseer::TextureFormatType::ABGR8;

	bool DDSTextureLoader::Load(void* data, int32_t size)
	{
		struct DDS_PIXELFORMAT {
			uint32_t dwSize;
			uint32_t dwFlags;
			uint32_t dwFourCC;
			uint32_t dwRGBBitCount;
			uint32_t dwRBitMask;
			uint32_t dwGBitMask;
			uint32_t dwBBitMask;
			uint32_t dwABitMask;
		};

		struct DDS_HEADER {
			uint32_t dwSize;
			uint32_t dwFlags;
			uint32_t dwHeight;
			uint32_t dwWidth;
			uint32_t dwPitchOrLinearSize;
			uint32_t dwDepth;
			uint32_t dwMipMapCount;
			uint32_t dwReserved1[11];
			DDS_PIXELFORMAT ddspf;
			uint32_t dwCaps1;
			uint32_t dwCaps2;
			uint32_t dwReserved2[3];
		};

		auto p = (uint8_t*)data;
		const uint32_t FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
		const uint32_t FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
		const uint32_t FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))

		if (size < 4 + sizeof(DDS_HEADER)) return false;

		if (p[0] == 'D' &&
			p[1] == 'D' &&
			p[2] == 'S' &&
			p[3] == ' ')
		{
			p += 4;
		}
		else
		{
			return false;
		}

		DDS_HEADER dds;
		memcpy(&dds, p, sizeof(DDS_HEADER));
		p += sizeof(DDS_HEADER);

		// Currently mipmap is unsupported.
		if (dds.dwMipMapCount != 0)
		{
			return false;
		}

		if (dds.ddspf.dwRGBBitCount == 32 &&
			dds.ddspf.dwRBitMask == 0x000000FF &&
			dds.ddspf.dwGBitMask == 0x0000FF00 &&
			dds.ddspf.dwBBitMask == 0x00FF0000 &&
			dds.ddspf.dwABitMask == 0xFF000000)
		{
			textureFormatType = Effekseer::TextureFormatType::ABGR8;
		}
		if (dds.ddspf.dwFourCC == FOURCC_DXT1)
		{
			textureFormatType = Effekseer::TextureFormatType::BC1;
		}
		else if (dds.ddspf.dwFourCC == FOURCC_DXT3)
		{
			textureFormatType = Effekseer::TextureFormatType::BC2;
		}
		else if (dds.ddspf.dwFourCC == FOURCC_DXT5)
		{
			textureFormatType = Effekseer::TextureFormatType::BC3;
		}
		else
		{
			return false;
		}

		textureData.resize(size - 4 - sizeof(DDS_HEADER));
		memcpy(&(textureData[0]), p, textureData.size());

		textureWidth = dds.dwWidth;
		textureHeight = dds.dwHeight;

		return true;
	}

	void DDSTextureLoader::Unload()
	{
		textureData.clear();
	}

}