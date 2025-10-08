
#ifndef __EFFEKSEERRENDERER_DX11_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_DX11_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Base.h"
#include "EffekseerRendererDX11.RendererImplemented.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	static const int32_t DepthTestCount = 2;
	static const int32_t DepthWriteCount = 2;
	static const int32_t CulTypeCount = 3;
	static const int32_t AlphaTypeCount = 5;
	static const int32_t TextureFilterCount = 2;
	static const int32_t TextureWrapCount = 3;

	RendererImplemented* renderer_ = nullptr;
	ID3D11RasterizerState* rasterizer_states_[CulTypeCount];
	ID3D11DepthStencilState* depth_stencil_states_[DepthTestCount][DepthWriteCount];
	ID3D11BlendState* blend_states_[AlphaTypeCount];
	ID3D11SamplerState* sampler_states_[TextureFilterCount][TextureWrapCount];

public:
	RenderState(RendererImplemented* renderer, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled);
	virtual ~RenderState();

	void Update(bool forced);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERSTATE_H__