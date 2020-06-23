
#ifndef __EFFEKSEERRENDERER_DX9_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_DX9_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRendererDX9.Base.h"

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
	RendererImplemented* m_renderer;

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