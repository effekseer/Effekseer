
#ifndef __EFFEKSEERRENDERER_DX9_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_DX9_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.Base.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	RendererImplemented* renderer_;

public:
	RenderState(RendererImplemented* renderer);
	virtual ~RenderState();

	void Update(bool forced);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_RENDERSTATE_H__