
#ifndef	__EFFEKSEERRENDERER_RENDERSTATE_H__
#define	__EFFEKSEERRENDERER_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.Base.h"
#include "EffekseerRenderer.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState
	: public RenderStateBase
{
private:
	static const int32_t		DepthTestCount = 2;
	static const int32_t		DepthWriteCount = 2;
	static const int32_t		CulTypeCount = 3;
	static const int32_t		AlphaTypeCount = 5;
	static const int32_t		TextureFilterCount = 2;
	static const int32_t		TextureWrapCount = 2;

	RendererImplemented*		m_renderer;
	ID3D11RasterizerState*		m_rStates[CulTypeCount];
	ID3D11DepthStencilState*	m_dStates[DepthTestCount][DepthWriteCount];
	ID3D11BlendState*			m_bStates[AlphaTypeCount];
	ID3D11SamplerState*			m_sStates[TextureFilterCount][TextureWrapCount];

public:
	RenderState( RendererImplemented* renderer );
	virtual ~RenderState();

	void Update( bool forced );
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERSTATE_H__