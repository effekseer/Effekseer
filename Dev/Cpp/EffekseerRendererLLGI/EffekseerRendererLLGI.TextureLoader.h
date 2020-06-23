#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef __EFFEKSEERRENDERER_LLGI_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_LLGI_TEXTURELOADER_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#include <LLGI.Graphics.h>
#include <LLGI.Texture.h>

namespace EffekseerRendererLLGI
{

class TextureLoader : public ::Effekseer::TextureLoader
{
private:
	GraphicsDevice* graphicsDevice_ = nullptr;

	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;
	::EffekseerRenderer::PngTextureLoader pngTextureLoader_;

public:
	TextureLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);
	virtual ~TextureLoader();

public:
	Effekseer::TextureData* Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override;

	void Unload(Effekseer::TextureData* data) override;
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_TEXTURELOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__