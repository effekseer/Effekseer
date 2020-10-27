
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_GL_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_GL_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;
	::Effekseer::ColorSpaceType colorSpaceType_;

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	::EffekseerRenderer::PngTextureLoader pngTextureLoader;
	::EffekseerRenderer::DDSTextureLoader ddsTextureLoader;
#endif

public:
	TextureLoader(::Effekseer::FileInterface* fileInterface = nullptr,
				  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);
	virtual ~TextureLoader();

public:
	Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;

	Effekseer::TextureData* Load(const void* data, int32_t size, Effekseer::TextureType textureType) override;

	void Unload(Effekseer::TextureData* data) override;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_TEXTURELOADER_H__

#endif