
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
	: renderer_(renderer)
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
	if (active_.DepthTest != next_.DepthTest || forced)
	{
		if (next_.DepthTest)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);
		}
		else
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
		}
	}

	if (active_.DepthWrite != next_.DepthWrite || forced)
	{
		if (next_.DepthWrite)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
	}

	if (active_.CullingType != next_.CullingType || forced)
	{
		if (next_.CullingType == ::Effekseer::CullingType::Front)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else if (next_.CullingType == ::Effekseer::CullingType::Back)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		}
		else if (next_.CullingType == ::Effekseer::CullingType::Double)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

	if (active_.AlphaBlend != next_.AlphaBlend || forced)
	{
		if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Opacity || renderer_->GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_MAX);
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Blend)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
			if (renderer_->GetImpl()->IsPremultipliedAlphaEnabled)
			{
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
			}
			else
			{
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
			}
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Add)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

			if (renderer_->GetImpl()->IsPremultipliedAlphaEnabled)
			{
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			}
			else
			{
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
				renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			}
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Sub)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
		else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Mul)
		{
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
			renderer_->GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
			renderer_->GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO);
			renderer_->GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
			renderer_->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);
		}
	}

	for (int32_t i = 0; i < Effekseer::TextureSlotMax; i++)
	{
		if (active_.TextureFilterTypes[i] != next_.TextureFilterTypes[i] || forced)
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

			int32_t filter_ = (int32_t)next_.TextureFilterTypes[i];

			// VTF is not supported

			renderer_->GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, MinFilterTable[filter_]);
			renderer_->GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, MagFilterTable[filter_]);
			renderer_->GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, MipFilterTable[filter_]);
		}

		if (active_.TextureWrapTypes[i] != next_.TextureWrapTypes[i] || forced)
		{
			std::array<int32_t, 3> wraps = {
				D3DTADDRESS_WRAP,
				D3DTADDRESS_CLAMP,
				D3DTADDRESS_MIRROR,
			};

			auto wrap = wraps[static_cast<int32_t>(next_.TextureWrapTypes[i])];

			// for VTF
			if (i < 4)
			{
				renderer_->GetDevice()->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, wrap);
				renderer_->GetDevice()->SetSamplerState(i + D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, wrap);
			}

			renderer_->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, wrap);
			renderer_->GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, wrap);
		}
	}

	active_ = next_;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
