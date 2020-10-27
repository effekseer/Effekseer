
#ifndef __EFFEKSEERRENDERER_DX11_RENDERSTATE_H__
#define __EFFEKSEERRENDERER_DX11_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRendererDX11.Base.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState
	: public ::EffekseerRenderer::RenderStateBase
{
private:
	static const int32_t DepthTestCount = 2;
	static const int32_t DepthWriteCount = 2;
	static const int32_t CulTypeCount = 3;
	static const int32_t AlphaTypeCount = 5;
	static const int32_t TextureFilterCount = 2;
	static const int32_t TextureWrapCount = 2;

	RendererImplemented* m_renderer;
	ID3D11RasterizerState* m_rStates[CulTypeCount];
	ID3D11DepthStencilState* m_dStates[DepthTestCount][DepthWriteCount];
	ID3D11BlendState* m_bStates[AlphaTypeCount];
	ID3D11SamplerState* m_sStates[TextureFilterCount][TextureWrapCount];

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