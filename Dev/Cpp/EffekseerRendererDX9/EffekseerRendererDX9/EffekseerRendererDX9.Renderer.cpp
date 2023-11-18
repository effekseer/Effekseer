
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.Renderer.h"
#include "EffekseerRendererDX9.RenderState.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

#include "EffekseerRendererDX9.MaterialLoader.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
#include "EffekseerRendererDX9.Shader.h"
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h>
#include <EffekseerRendererCommon/ModelLoader.h>
#include <EffekseerRendererCommon/TextureLoader.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{

namespace Standard_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_unlit_vs.h"
} // namespace Standard_VS_Ad

namespace Standard_PS_Ad
{
static
#include "ShaderHeader/ad_model_unlit_ps.h"
} // namespace Standard_PS_Ad

namespace Standard_Distortion_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_distortion_vs.h"
} // namespace Standard_Distortion_VS_Ad

namespace Standard_Distortion_PS_Ad
{
static
#include "ShaderHeader/ad_model_distortion_ps.h"
} // namespace Standard_Distortion_PS_Ad

namespace Standard_Lighting_VS_Ad
{
static
#include "ShaderHeader/ad_sprite_lit_vs.h"
} // namespace Standard_Lighting_VS_Ad

namespace Standard_Lighting_PS_Ad
{
static
#include "ShaderHeader/ad_model_lit_ps.h"
} // namespace Standard_Lighting_PS_Ad

namespace Standard_VS
{
static
#include "ShaderHeader/sprite_unlit_vs.h"
} // namespace Standard_VS

namespace Standard_PS
{
static
#include "ShaderHeader/model_unlit_ps.h"
} // namespace Standard_PS

namespace Standard_Distortion_VS
{
static
#include "ShaderHeader/sprite_distortion_vs.h"
} // namespace Standard_Distortion_VS

namespace Standard_Distortion_PS
{
static
#include "ShaderHeader/model_distortion_ps.h"
} // namespace Standard_Distortion_PS

namespace Standard_Lighting_VS
{
static
#include "ShaderHeader/sprite_lit_vs.h"
} // namespace Standard_Lighting_VS

namespace Standard_Lighting_PS
{
static
#include "ShaderHeader/model_lit_ps.h"
} // namespace Standard_Lighting_PS

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(LPDIRECT3DDEVICE9 device)
{
	return Effekseer::MakeRefPtr<Backend::GraphicsDevice>(device);
}

RendererRef Renderer::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount)
{
	auto renderer = ::Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount);
	if (renderer->Initialize(graphicsDevice.DownCast<Backend::GraphicsDevice>()))
	{
		return renderer;
	}
	return nullptr;
}

RendererRef Renderer::Create(LPDIRECT3DDEVICE9 device, int32_t squareMaxCount)
{
	auto gd = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(device);
	return Create(gd, squareMaxCount);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_state_vertexShader(nullptr)
	, m_state_pixelShader(nullptr)
	, m_state_vertexDeclaration(nullptr)
	, m_state_IndexData(nullptr)
	, m_state_pTexture({})
	, m_renderState(nullptr)
	, m_isChangedDevice(false)
	, m_restorationOfStates(true)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_state_streamData.fill(nullptr);
	m_state_OffsetInBytes.fill(0);
	m_state_pStride.fill(0);

	SetRestorationOfStatesFlag(m_restorationOfStates);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	GetImpl()->DeleteProxyTextures(this);

	assert(GetRef() == 0);

	ES_SAFE_DELETE(m_distortingCallback);
	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_renderState);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
	GetImpl()->DeleteProxyTextures(this);
	SetBackground(nullptr);

	graphicsDevice_->LostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnResetDevice()
{
	graphicsDevice_->ResetDevice();

	if (m_isChangedDevice)
	{
		EffekseerRenderer::GenerateIndexDataStride<int16_t>(graphicsDevice_, m_squareMaxCount, indexBuffer_, indexBufferForWireframe_);

		m_isChangedDevice = false;
	}

	GetImpl()->CreateProxyTextures(this);
}

bool RendererImplemented::Initialize(Backend::GraphicsDeviceRef graphicsDevice)
{
	graphicsDevice_ = graphicsDevice;

	// generate a vertex buffer
	{
		GetImpl()->InternalVertexBuffer = std::make_shared<EffekseerRenderer::VertexBufferRing>(graphicsDevice_, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, 1);
		if (!GetImpl()->InternalVertexBuffer->GetIsValid())
		{
			GetImpl()->InternalVertexBuffer = nullptr;
			return false;
		}
	}

	if (!EffekseerRenderer::GenerateIndexDataStride<int16_t>(graphicsDevice_, m_squareMaxCount, indexBuffer_, indexBufferForWireframe_))
	{
		return false;
	}

	m_renderState = new RenderState(this);

	auto vlUnlit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Unlit).DownCast<Backend::VertexLayout>();
	auto vlLit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Lit).DownCast<Backend::VertexLayout>();
	auto vlDist = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::BackDistortion).DownCast<Backend::VertexLayout>();
	auto vlUnlitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedUnlit).DownCast<Backend::VertexLayout>();
	auto vlLitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedLit).DownCast<Backend::VertexLayout>();
	auto vlDistAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedBackDistortion).DownCast<Backend::VertexLayout>();

	vlUnlit->MakeGenerated();
	vlLit->MakeGenerated();
	vlDist->MakeGenerated();
	vlUnlitAd->MakeGenerated();
	vlLitAd->MakeGenerated();
	vlDistAd->MakeGenerated();

	auto shader_ad_unlit = Shader::Create(graphicsDevice_,
										  graphicsDevice_->CreateShaderFromBinary(
											  Standard_VS_Ad::g_vs30_main,
											  sizeof(Standard_VS_Ad::g_vs30_main),
											  Standard_PS_Ad::g_ps30_main,
											  sizeof(Standard_PS_Ad::g_ps30_main)),
										  vlUnlitAd);
	if (shader_ad_unlit == nullptr)
		return false;
	GetImpl()->ShaderAdUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_unlit);

	auto shader_unlit = Shader::Create(graphicsDevice_,
									   graphicsDevice_->CreateShaderFromBinary(
										   Standard_VS::g_vs30_main,
										   sizeof(Standard_VS::g_vs30_main),
										   Standard_PS::g_ps30_main,
										   sizeof(Standard_PS::g_ps30_main)),
									   vlUnlit);
	if (shader_unlit == nullptr)
		return false;
	GetImpl()->ShaderUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_unlit);

	auto shader_ad_distortion = Shader::Create(graphicsDevice_,
											   graphicsDevice_->CreateShaderFromBinary(

												   Standard_Distortion_VS_Ad::g_vs30_main,
												   sizeof(Standard_Distortion_VS_Ad::g_vs30_main),
												   Standard_Distortion_PS_Ad::g_ps30_main,
												   sizeof(Standard_Distortion_PS_Ad::g_ps30_main)),
											   vlDistAd);
	if (shader_ad_distortion == nullptr)
		return false;
	GetImpl()->ShaderAdDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_distortion);

	auto shader_distortion = Shader::Create(graphicsDevice_,
											graphicsDevice_->CreateShaderFromBinary(

												Standard_Distortion_VS::g_vs30_main,
												sizeof(Standard_Distortion_VS::g_vs30_main),
												Standard_Distortion_PS::g_ps30_main,
												sizeof(Standard_Distortion_PS::g_ps30_main)),
											vlDist);
	if (shader_distortion == nullptr)
		return false;
	GetImpl()->ShaderDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_distortion);

	auto shader_ad_lit = Shader::Create(graphicsDevice_,
										graphicsDevice_->CreateShaderFromBinary(

											Standard_Lighting_VS_Ad::g_vs30_main,
											sizeof(Standard_Lighting_VS_Ad::g_vs30_main),
											Standard_Lighting_PS_Ad::g_ps30_main,
											sizeof(Standard_Lighting_PS_Ad::g_ps30_main)),
										vlLitAd);
	if (shader_ad_lit == nullptr)
		return false;
	GetImpl()->ShaderAdLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_lit);

	auto shader_lit = Shader::Create(graphicsDevice_,
									 graphicsDevice_->CreateShaderFromBinary(

										 Standard_Lighting_VS::g_vs30_main,
										 sizeof(Standard_Lighting_VS::g_vs30_main),
										 Standard_Lighting_PS::g_ps30_main,
										 sizeof(Standard_Lighting_PS::g_ps30_main)),
									 vlLit);
	if (shader_lit == nullptr)
		return false;
	GetImpl()->ShaderLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_lit);

	shader_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_ad_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBufferDistortion));

	shader_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	shader_ad_lit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
	shader_ad_lit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::PixelConstantBuffer));

	m_standardRenderer =
		new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>(this);

	GetImpl()->CreateProxyTextures(this);

	std::array<float, 64> instancedVertex;
	for (size_t i = 0; i < instancedVertex.size(); i++)
	{
		instancedVertex[i] = static_cast<float>(i);
	}

	instancedVertexBuffer_ = graphicsDevice_->CreateVertexBuffer((int32_t)(instancedVertex.size() * sizeof(float)), instancedVertex.data(), false);
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	m_restorationOfStates = flag;
	if (flag)
	{
		m_state_VertexShaderConstantF.resize(256 * 4);
		m_state_PixelShaderConstantF.resize(256 * 4);
	}
	else
	{
		m_state_VertexShaderConstantF.clear();
		m_state_PixelShaderConstantF.shrink_to_fit();
		m_state_VertexShaderConstantF.clear();
		m_state_PixelShaderConstantF.shrink_to_fit();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	impl->CalculateCameraProjectionMatrix();

	// ステートを保存する
	if (m_restorationOfStates)
	{
		GetDevice()->GetRenderState(D3DRS_ALPHABLENDENABLE, &m_state_D3DRS_ALPHABLENDENABLE);
		GetDevice()->GetRenderState(D3DRS_BLENDOP, &m_state_D3DRS_BLENDOP);
		GetDevice()->GetRenderState(D3DRS_DESTBLEND, &m_state_D3DRS_DESTBLEND);
		GetDevice()->GetRenderState(D3DRS_SRCBLEND, &m_state_D3DRS_SRCBLEND);
		GetDevice()->GetRenderState(D3DRS_ALPHAREF, &m_state_D3DRS_ALPHAREF);

		GetDevice()->GetRenderState(D3DRS_DESTBLENDALPHA, &m_state_D3DRS_DESTBLENDALPHA);
		GetDevice()->GetRenderState(D3DRS_SRCBLENDALPHA, &m_state_D3DRS_SRCBLENDALPHA);
		GetDevice()->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, &m_state_D3DRS_SEPARATEALPHABLENDENABLE);
		GetDevice()->GetRenderState(D3DRS_BLENDOPALPHA, &m_state_D3DRS_BLENDOPALPHA);

		GetDevice()->GetRenderState(D3DRS_ZENABLE, &m_state_D3DRS_ZENABLE);
		GetDevice()->GetRenderState(D3DRS_ZWRITEENABLE, &m_state_D3DRS_ZWRITEENABLE);
		GetDevice()->GetRenderState(D3DRS_ALPHATESTENABLE, &m_state_D3DRS_ALPHATESTENABLE);
		GetDevice()->GetRenderState(D3DRS_CULLMODE, &m_state_D3DRS_CULLMODE);

		GetDevice()->GetRenderState(D3DRS_COLORVERTEX, &m_state_D3DRS_COLORVERTEX);
		GetDevice()->GetRenderState(D3DRS_LIGHTING, &m_state_D3DRS_LIGHTING);
		GetDevice()->GetRenderState(D3DRS_SHADEMODE, &m_state_D3DRS_SHADEMODE);

		for (int i = 0; i < static_cast<int>(m_state_D3DSAMP_MAGFILTER.size()); i++)
		{
			GetDevice()->GetSamplerState(i, D3DSAMP_MAGFILTER, &m_state_D3DSAMP_MAGFILTER[i]);
			GetDevice()->GetSamplerState(i, D3DSAMP_MINFILTER, &m_state_D3DSAMP_MINFILTER[i]);
			GetDevice()->GetSamplerState(i, D3DSAMP_MIPFILTER, &m_state_D3DSAMP_MIPFILTER[i]);
			GetDevice()->GetSamplerState(i, D3DSAMP_ADDRESSU, &m_state_D3DSAMP_ADDRESSU[i]);
			GetDevice()->GetSamplerState(i, D3DSAMP_ADDRESSV, &m_state_D3DSAMP_ADDRESSV[i]);
		}

		GetDevice()->GetVertexShader(&m_state_vertexShader);
		GetDevice()->GetPixelShader(&m_state_pixelShader);
		GetDevice()->GetVertexDeclaration(&m_state_vertexDeclaration);

		for (size_t i = 0; i < m_state_streamData.size(); i++)
		{
			GetDevice()->GetStreamSource((UINT)i, &m_state_streamData[i], &m_state_OffsetInBytes[i], &m_state_pStride[i]);
			GetDevice()->GetStreamSourceFreq((UINT)i, &m_state_streamFreq[i]);
		}

		GetDevice()->GetIndices(&m_state_IndexData);

		GetDevice()->GetVertexShaderConstantF(
			0, m_state_VertexShaderConstantF.data(), static_cast<int>(m_state_VertexShaderConstantF.size()) / 4);
		GetDevice()->GetVertexShaderConstantF(
			0, m_state_PixelShaderConstantF.data(), static_cast<int>(m_state_PixelShaderConstantF.size()) / 4);

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->GetTexture(i, &m_state_pTexture[i]);
		}
		GetDevice()->GetFVF(&m_state_FVF);
	}

	// ステート初期値設定
	GetDevice()->SetTexture(0, nullptr);
	GetDevice()->SetFVF(0);

	GetDevice()->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	GetDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	// ステートを復元する
	if (m_restorationOfStates)
	{
		GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, m_state_D3DRS_ALPHABLENDENABLE);
		GetDevice()->SetRenderState(D3DRS_BLENDOP, m_state_D3DRS_BLENDOP);
		GetDevice()->SetRenderState(D3DRS_DESTBLEND, m_state_D3DRS_DESTBLEND);
		GetDevice()->SetRenderState(D3DRS_SRCBLEND, m_state_D3DRS_SRCBLEND);
		GetDevice()->SetRenderState(D3DRS_ALPHAREF, m_state_D3DRS_ALPHAREF);

		GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, m_state_D3DRS_DESTBLENDALPHA);
		GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, m_state_D3DRS_SRCBLENDALPHA);
		GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, m_state_D3DRS_SEPARATEALPHABLENDENABLE);
		GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, m_state_D3DRS_BLENDOPALPHA);

		GetDevice()->SetRenderState(D3DRS_ZENABLE, m_state_D3DRS_ZENABLE);
		GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, m_state_D3DRS_ZWRITEENABLE);
		GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, m_state_D3DRS_ALPHATESTENABLE);
		GetDevice()->SetRenderState(D3DRS_CULLMODE, m_state_D3DRS_CULLMODE);

		GetDevice()->SetRenderState(D3DRS_COLORVERTEX, m_state_D3DRS_COLORVERTEX);
		GetDevice()->SetRenderState(D3DRS_LIGHTING, m_state_D3DRS_LIGHTING);
		GetDevice()->SetRenderState(D3DRS_SHADEMODE, m_state_D3DRS_SHADEMODE);

		for (int i = 0; i < static_cast<int>(m_state_D3DSAMP_MAGFILTER.size()); i++)
		{
			GetDevice()->SetSamplerState(i, D3DSAMP_MAGFILTER, m_state_D3DSAMP_MAGFILTER[i]);
			GetDevice()->SetSamplerState(i, D3DSAMP_MINFILTER, m_state_D3DSAMP_MINFILTER[i]);
			GetDevice()->SetSamplerState(i, D3DSAMP_MIPFILTER, m_state_D3DSAMP_MIPFILTER[i]);
			GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSU, m_state_D3DSAMP_ADDRESSU[i]);
			GetDevice()->SetSamplerState(i, D3DSAMP_ADDRESSV, m_state_D3DSAMP_ADDRESSV[i]);
		}

		GetDevice()->SetVertexShader(m_state_vertexShader);
		ES_SAFE_RELEASE(m_state_vertexShader);

		GetDevice()->SetPixelShader(m_state_pixelShader);
		ES_SAFE_RELEASE(m_state_pixelShader);

		GetDevice()->SetVertexDeclaration(m_state_vertexDeclaration);
		ES_SAFE_RELEASE(m_state_vertexDeclaration);

		for (size_t i = 0; i < m_state_streamData.size(); i++)
		{
			GetDevice()->SetStreamSource((UINT)i, m_state_streamData[i], m_state_OffsetInBytes[i], m_state_pStride[i]);
			ES_SAFE_RELEASE(m_state_streamData[i]);
			GetDevice()->SetStreamSourceFreq((UINT)i, m_state_streamFreq[i]);
		}

		GetDevice()->SetIndices(m_state_IndexData);
		ES_SAFE_RELEASE(m_state_IndexData);

		GetDevice()->SetVertexShaderConstantF(0, m_state_VertexShaderConstantF.data(), (UINT)m_state_VertexShaderConstantF.size() / 4);
		GetDevice()->SetVertexShaderConstantF(0, m_state_PixelShaderConstantF.data(), (UINT)m_state_PixelShaderConstantF.size() / 4);

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->SetTexture(i, m_state_pTexture[i]);
			ES_SAFE_RELEASE(m_state_pTexture[i]);
		}

		GetDevice()->SetFVF(m_state_FVF);
	}

	currentndexBuffer_ = nullptr;

	return true;
}

Effekseer::Backend::IndexBufferRef RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return indexBufferForWireframe_;
	}
	return indexBuffer_;
}

int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, true>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, true>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, true>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(RendererImplementedRef::FromPinned(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, true>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoaderRef RendererImplemented::CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::EffekseerRenderer::CreateTextureLoader(graphicsDevice_, fileInterface, ::Effekseer::ColorSpaceType::Gamma);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoaderRef RendererImplemented::CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<EffekseerRenderer::ModelLoader>(graphicsDevice_, fileInterface);
}

::Effekseer::MaterialLoaderRef RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice_, fileInterface);
}

void RendererImplemented::SetBackground(IDirect3DTexture9* background)
{
	if (background == nullptr)
	{
		m_backgroundDX9.Reset();
		EffekseerRenderer::Renderer::SetBackground(nullptr);
		return;
	}

	if (m_backgroundDX9 == nullptr)
	{
		m_backgroundDX9 = graphicsDevice_->CreateTexture(
			background, [](auto texture) -> auto {}, [](auto texture) -> auto {});
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(m_backgroundDX9.Get());
		texture->Init(
			background, [](auto texture) -> auto {}, [](auto texture) -> auto {});
	}

	EffekseerRenderer::Renderer::SetBackground(m_backgroundDX9);
}

EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return m_distortingCallback;
}

void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
	ES_SAFE_DELETE(m_distortingCallback);
	m_distortingCallback = callback;
}

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size)
{
	auto vb = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());
	GetDevice()->SetStreamSource(0, vb->GetBuffer(), 0, size);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ib = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
	currentndexBuffer_ = indexBuffer;
	GetDevice()->SetIndices(ib->GetBuffer());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	GetDevice()->SetVertexDeclaration(shader->GetLayoutInterface());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	if (GetRenderMode() == ::Effekseer::RenderMode::Normal)
	{
		GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertexOffset, 0, spriteCount * 4, 0, spriteCount * 2);
	}
	else if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		GetDevice()->DrawIndexedPrimitive(D3DPT_LINELIST, vertexOffset, 0, spriteCount * 4, 0, spriteCount * 4);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;

	auto vb = static_cast<Backend::VertexBuffer*>(instancedVertexBuffer_.Get());
	GetDevice()->SetStreamSource(1, vb->GetBuffer(), 0, sizeof(float));
	GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | 1);
	GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, indexCount / 3);
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;

	auto vb = static_cast<Backend::VertexBuffer*>(instancedVertexBuffer_.Get());
	GetDevice()->SetStreamSource(1, vb->GetBuffer(), 0, sizeof(float));
	GetDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | instanceCount);
	GetDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1);

	GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, indexCount / 3);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	currentShader = shader;
	GetDevice()->SetVertexShader(shader->GetVertexShader());
	GetDevice()->SetPixelShader(shader->GetPixelShader());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	currentShader = nullptr;
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count)
{
	for (int32_t i = 0; i < count; i++)
	{
		// For VTF
		if (i < 4)
		{
			if (textures[i] == nullptr)
			{
				GetDevice()->SetTexture(i + D3DVERTEXTEXTURESAMPLER0, nullptr);
			}
			else
			{
				auto texture = static_cast<Backend::Texture*>(textures[i].Get())->GetTexture();
				GetDevice()->SetTexture(i + D3DVERTEXTEXTURESAMPLER0, texture);
			}
		}

		if (textures[i] == nullptr)
		{
			GetDevice()->SetTexture(i, nullptr);
		}
		else
		{
			auto texture = static_cast<Backend::Texture*>(textures[i].Get())->GetTexture();
			GetDevice()->SetTexture(i, texture);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ChangeDevice(LPDIRECT3DDEVICE9 device)
{
	graphicsDevice_->ChangeDevice(device);

	m_isChangedDevice = true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

LPDIRECT3DDEVICE9 RendererImplemented::GetDevice()
{
	return graphicsDevice_->GetDevice();
}

Effekseer::Backend::GraphicsDeviceRef RendererImplemented::GetGraphicsDevice() const
{
	return graphicsDevice_;
}

} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
