
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RenderState.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::RenderState(RendererImplemented* renderer, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled)
	: m_renderer(renderer)
{
	D3D11_CULL_MODE cullTbl[] = {
		D3D11_CULL_BACK,
		D3D11_CULL_FRONT,
		D3D11_CULL_NONE,
	};

	for (int32_t ct = 0; ct < CulTypeCount; ct++)
	{
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.CullMode = cullTbl[ct];
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.DepthClipEnable = TRUE;

		if (isMSAAEnabled)
		{
			rsDesc.MultisampleEnable = TRUE;
		}

		rsDesc.FrontCounterClockwise =
			(depthFunc == D3D11_COMPARISON_GREATER_EQUAL || depthFunc == D3D11_COMPARISON_GREATER) ? TRUE : FALSE;
		m_renderer->GetDevice()->CreateRasterizerState(&rsDesc, &m_rStates[ct]);
	}

	for (int32_t dt = 0; dt < DepthTestCount; dt++)
	{
		for (int32_t dw = 0; dw < DepthWriteCount; dw++)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;
			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = dt;
			dsDesc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK)dw;
			dsDesc.DepthFunc = depthFunc;
			dsDesc.StencilEnable = FALSE;
			m_renderer->GetDevice()->CreateDepthStencilState(&dsDesc, &m_dStates[dt][dw]);
		}
	}

	for (int32_t i = 0; i < AlphaTypeCount; i++)
	{
		auto type = (::Effekseer::AlphaBlendType)i;

		if (m_renderer->GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
			type = ::Effekseer::AlphaBlendType::Opacity;

		D3D11_BLEND_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.AlphaToCoverageEnable = false;

		for (int32_t k = 0; k < 8; k++)
		{
			Desc.RenderTarget[k].BlendEnable = true;
			Desc.RenderTarget[k].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			Desc.RenderTarget[k].SrcBlendAlpha = D3D11_BLEND_ONE;
			Desc.RenderTarget[k].DestBlendAlpha = D3D11_BLEND_ONE;
			Desc.RenderTarget[k].BlendOpAlpha = D3D11_BLEND_OP_MAX;

			switch (type)
			{
			case ::Effekseer::AlphaBlendType::Opacity:
				Desc.RenderTarget[k].DestBlend = D3D11_BLEND_ZERO;
				Desc.RenderTarget[k].SrcBlend = D3D11_BLEND_ONE;
				Desc.RenderTarget[k].BlendOp = D3D11_BLEND_OP_ADD;
				break;
			case ::Effekseer::AlphaBlendType::Blend:
				Desc.RenderTarget[k].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				Desc.RenderTarget[k].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				Desc.RenderTarget[k].BlendOp = D3D11_BLEND_OP_ADD;
				break;
			case ::Effekseer::AlphaBlendType::Add:
				Desc.RenderTarget[k].DestBlend = D3D11_BLEND_ONE;
				Desc.RenderTarget[k].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				Desc.RenderTarget[k].BlendOp = D3D11_BLEND_OP_ADD;
				break;
			case ::Effekseer::AlphaBlendType::Sub:
				Desc.RenderTarget[k].DestBlend = D3D11_BLEND_ONE;
				Desc.RenderTarget[k].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				Desc.RenderTarget[k].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
				Desc.RenderTarget[k].SrcBlendAlpha = D3D11_BLEND_ZERO;
				Desc.RenderTarget[k].DestBlendAlpha = D3D11_BLEND_ONE;
				Desc.RenderTarget[k].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				break;

			case ::Effekseer::AlphaBlendType::Mul:
				Desc.RenderTarget[k].DestBlend = D3D11_BLEND_SRC_COLOR;
				Desc.RenderTarget[k].SrcBlend = D3D11_BLEND_ZERO;
				Desc.RenderTarget[k].BlendOp = D3D11_BLEND_OP_ADD;
				Desc.RenderTarget[k].SrcBlendAlpha = D3D11_BLEND_ZERO;
				Desc.RenderTarget[k].DestBlendAlpha = D3D11_BLEND_ONE;
				Desc.RenderTarget[k].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				break;
			}
		}

		m_renderer->GetDevice()->CreateBlendState(&Desc, &m_bStates[i]);
	}

	for (int32_t f = 0; f < TextureFilterCount; f++)
	{
		for (int32_t w = 0; w < TextureWrapCount; w++)
		{
			D3D11_TEXTURE_ADDRESS_MODE Addres[] = {
				D3D11_TEXTURE_ADDRESS_WRAP,
				D3D11_TEXTURE_ADDRESS_CLAMP,
			};

			D3D11_FILTER Filter[] = {
				D3D11_FILTER_MIN_MAG_MIP_POINT,
				D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
			};

			uint32_t Anisotropic[] = {
				0,
				0,
			};

			D3D11_SAMPLER_DESC SamlerDesc = {
				Filter[f],
				Addres[w],
				Addres[w],
				Addres[w],
				0.0f,
				Anisotropic[f],
				D3D11_COMPARISON_ALWAYS,
				{0.0f, 0.0f, 0.0f, 0.0f},
				0.0f,
				D3D11_FLOAT32_MAX,
			};

			m_renderer->GetDevice()->CreateSamplerState(&SamlerDesc, &m_sStates[f][w]);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::~RenderState()
{
	for (int32_t ct = 0; ct < CulTypeCount; ct++)
	{
		ES_SAFE_RELEASE(m_rStates[ct]);
	}

	for (int32_t dt = 0; dt < DepthTestCount; dt++)
	{
		for (int32_t dw = 0; dw < DepthWriteCount; dw++)
		{
			ES_SAFE_RELEASE(m_dStates[dt][dw]);
		}
	}

	for (int32_t i = 0; i < AlphaTypeCount; i++)
	{
		ES_SAFE_RELEASE(m_bStates[i]);
	}

	for (int32_t f = 0; f < TextureFilterCount; f++)
	{
		for (int32_t w = 0; w < TextureWrapCount; w++)
		{
			ES_SAFE_RELEASE(m_sStates[f][w]);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderState::Update(bool forced)
{
	bool changeDepth = forced;
	bool changeRasterizer = forced;
	bool changeBlend = forced;

	if (m_active.DepthTest != m_next.DepthTest || forced)
	{
		changeDepth = true;
	}

	if (m_active.DepthWrite != m_next.DepthWrite || forced)
	{
		changeDepth = true;
	}

	if (changeDepth)
	{
		m_renderer->GetContext()->OMSetDepthStencilState(m_dStates[m_next.DepthTest][m_next.DepthWrite], 0);
	}

	if (m_active.CullingType != m_next.CullingType || forced)
	{
		changeRasterizer = true;
	}

	if (changeRasterizer)
	{
		auto cullingType = (int32_t)m_next.CullingType;
		m_renderer->GetContext()->RSSetState(m_rStates[cullingType]);
	}

	if (m_active.AlphaBlend != m_next.AlphaBlend || forced)
	{
		changeBlend = true;
	}

	if (changeBlend)
	{
		auto alphaBlend = (int32_t)m_next.AlphaBlend;
		float blendFactor[] = {0, 0, 0, 0};
		m_renderer->GetContext()->OMSetBlendState(m_bStates[alphaBlend], blendFactor, 0xFFFFFFFF);
	}

	for (int32_t i = 0; i < Effekseer::TextureSlotMax; i++)
	{
		bool changeSampler = forced;

		if (m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (changeSampler)
		{
			auto filter = (int32_t)m_next.TextureFilterTypes[i];
			auto wrap = (int32_t)m_next.TextureWrapTypes[i];

			ID3D11SamplerState* samplerTbl[] = {m_sStates[filter][wrap]};
			m_renderer->GetContext()->VSSetSamplers(i, 1, samplerTbl);
			m_renderer->GetContext()->PSSetSamplers(i, 1, samplerTbl);
		}
	}

	m_active = m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
