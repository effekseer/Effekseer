
#ifndef	__EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__
#define	__EFFEKSEERRENDERER_PNG_TEXTURE_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <vector>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
	class PngTextureLoader
	{
	private:
		static std::vector<uint8_t> textureData;
		static int32_t textureWidth;
		static int32_t textureHeight;
	public:

		static bool Load(void* data, int32_t size, bool rev);
		static void Unload();

		static void Initialize();
		static void Finalize();

		static std::vector<uint8_t>& GetData() { return textureData; }
		static int32_t GetWidth() { return textureWidth; }
		static int32_t GetHeight() { return textureHeight; }
	};
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

#endif
