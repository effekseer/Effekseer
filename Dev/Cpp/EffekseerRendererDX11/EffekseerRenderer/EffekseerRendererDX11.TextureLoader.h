#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class TextureLoader
	: public ::Effekseer::TextureLoader
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context = nullptr;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;
	::Effekseer::ColorSpaceType colorSpaceType_;

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	::EffekseerRenderer::PngTextureLoader pngTextureLoader;
	::EffekseerRenderer::DDSTextureLoader ddsTextureLoader;
#endif

public:
	TextureLoader(ID3D11Device* device,
				  ID3D11DeviceContext* context,
				  ::Effekseer::FileInterface* fileInterface = nullptr,
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
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_TEXTURELOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__