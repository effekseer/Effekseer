
#ifndef __EFFEKSEERRENDERER_PASTE_H__
#define __EFFEKSEERRENDERER_PASTE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Graphics/efk.ImageRenderer.h"
#include "EffekseerTool.Renderer.h"

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
	efk::ImageRenderer* imageRenderer = nullptr;

	Paste(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	virtual ~Paste();

	static Paste* Create(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	void Rendering(::Effekseer::TextureData* texture, int32_t width, int32_t height);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_PASTE_H__