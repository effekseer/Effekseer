
#ifndef __EFFEKSEERRENDERER_GL_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_GL_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	RendererImplemented* renderer_;
	bool isCCW_ = true;

	std::array<GLuint, Effekseer::TextureSlotMax> samplers_;

public:
	RenderState(RendererImplemented* renderer);
	virtual ~RenderState();

	void Update(bool forced);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERSTATE_H__