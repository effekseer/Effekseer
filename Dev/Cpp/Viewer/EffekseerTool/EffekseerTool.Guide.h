
#ifndef __EFFEKSEERRENDERER_GUIDE_H__
#define __EFFEKSEERRENDERER_GUIDE_H__

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
class Guide
{
private:
	efk::ImageRenderer* imageRenderer = nullptr;

	Guide(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	virtual ~Guide();

	static Guide* Create(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	void Rendering(int32_t width, int32_t height, int32_t guide_width, int32_t guide_height);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GUIDE_H__