
#ifndef __EFFEKSEERRENDERER_PASTE_H__
#define __EFFEKSEERRENDERER_PASTE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Graphics/efk.ImageRenderer.h"
#include "EffekseerTool.Renderer.h"
#include "../Graphics/ImageRenderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Paste
{
private:
	std::shared_ptr<Effekseer::Tool::ImageRenderer> imageRenderer_ = nullptr;

	Paste(efk::Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);

public:
	virtual ~Paste();

	static Paste* Create(efk::Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);

public:
	void Rendering(::Effekseer::TextureRef texture, int32_t width, int32_t height);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_PASTE_H__