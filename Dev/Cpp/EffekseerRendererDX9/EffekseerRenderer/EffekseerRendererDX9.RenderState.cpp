
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::RenderState(RendererImplemented* renderer)
	: m_renderer(renderer)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::~RenderState()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderState::Update(bool forced)
{
	if (m_active.DepthTest != m_next.DepthTest || forced)
	{
		if (m_next.DepthTest)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
		}
		else
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
		}
	}

	if (m_active.DepthWrite != m_next.DepthWrite || forced)
	{
		if (m_next.DepthWrite)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
	}

	if (m_active.CullingType != m_next.CullingType || forced)
	{
		if (m_next.CullingType == ::Effekseer::CullingType::Front)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else if (m_next.CullingType == ::Effekseer::CullingType::Back)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
		else if (m_next.CullingType == ::Effekseer::CullingType::Double)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

	if (m_active.AlphaBlend != m_next.AlphaBlend || forced)
	{
		if (m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Opacity ||
			m_renderer->GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_MAX);
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Blend)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Add)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Sub)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Mul)
		{
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
			m_renderer->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			m_renderer->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
			m_renderer->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
			m_renderer->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
	}

	for (int32_t i = 0; i < Effekseer::TextureSlotMax; i++)
	{
		if (m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced)
		{
			const uint32_t MinFilterTable[] = {
				D3DTEXF_POINT,
				D3DTEXF_LINEAR,
			};

			const uint32_t MagFilterTable[] = {
				D3DTEXF_POINT,
				D3DTEXF_LINEAR,
			};

			const uint32_t MipFilterTable[] = {
				D3DTEXF_NONE,
				D3DTEXF_LINEAR,
			};

			int32_t filter_ = (int32_t)m_next.TextureFilterTypes[i];

			// VTF is not supported

			m_renderer->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, MinFilterTable[filter_]);
			m_renderer->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, MagFilterTable[filter_]);
			m_renderer->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, MipFilterTable[filter_]);
		}

		if (m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced)
		{
			// for VTF
			if (i < 4)
			{
				m_renderer->GetDevice()->SetSamplerState(
					i + D3DVERTEXTEXTURESAMPLER0,
					D3DSAMP_ADDRESSU,
					m_next.TextureWrapTypes[i] == ::Effekseer::TextureWrapType::Repeat ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);

				m_renderer->GetDevice()->SetSamplerState(
					i + D3DVERTEXTEXTURESAMPLER0,
					D3DSAMP_ADDRESSV,
					m_next.TextureWrapTypes[i] == ::Effekseer::TextureWrapType::Repeat ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
			}

			m_renderer->GetDevice()->SetSamplerState(
				i,
				D3DSAMP_ADDRESSU,
				m_next.TextureWrapTypes[i] == ::Effekseer::TextureWrapType::Repeat ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);

			m_renderer->GetDevice()->SetSamplerState(
				i,
				D3DSAMP_ADDRESSV,
				m_next.TextureWrapTypes[i] == ::Effekseer::TextureWrapType::Repeat ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP);
		}
	}

	m_active = m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
