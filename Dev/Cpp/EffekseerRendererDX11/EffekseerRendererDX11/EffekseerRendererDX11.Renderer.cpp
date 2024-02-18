
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Renderer.h"
#include "EffekseerRendererDX11.RenderState.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.GpuTimer.h"
#include "EffekseerRendererDX11.GpuParticles.h"
#include "EffekseerRendererDX11.MaterialLoader.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
#include "EffekseerRendererDX11.Shader.h"

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
namespace EffekseerRendererDX11
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

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(ID3D11Device* device,
															 ID3D11DeviceContext* context)
{
	return Effekseer::MakeRefPtr<Backend::GraphicsDevice>(device, context);
}

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<MaterialLoader>(graphicsDevice, fileInterface);
}

::Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	auto gd = gprahicsDevice.DownCast<Backend::GraphicsDevice>();
	return gd->CreateTexture(srv, rtv, dsv);
}

TextureProperty GetTextureProperty(::Effekseer::Backend::TextureRef texture)
{
	if (texture != nullptr)
	{
		auto t = texture.DownCast<Backend::Texture>();
		return TextureProperty{t->GetSRV(), t->GetRTV(), t->GetDSV()};
	}
	else
	{
		return TextureProperty{};
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
OriginalState::OriginalState()
	: m_blendState(nullptr)
	, m_depthStencilState(nullptr)
	, m_depthStencilStateRef(0)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
	, m_layout(nullptr)

	, m_pRasterizerState(nullptr)
	, m_pVS(nullptr)
	, m_pPS(nullptr)
	, m_pVB(nullptr)
	, m_pIB(nullptr)
{
	m_samplers.fill(nullptr);
	m_psSRVs.fill(nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
OriginalState::~OriginalState()
{
	ReleaseState();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void OriginalState::SaveState(ID3D11Device* device, ID3D11DeviceContext* context)
{
	context->PSGetSamplers(0, Effekseer::TextureSlotMax, m_samplers.data());
	context->OMGetBlendState(&m_blendState, m_blendFactor.data(), &m_blendSampleMask);
	context->OMGetDepthStencilState(&m_depthStencilState, &m_depthStencilStateRef);
	context->RSGetState(&m_pRasterizerState);

	context->VSGetConstantBuffers(0, 1, &m_vertexConstantBuffer);
	context->PSGetConstantBuffers(0, 1, &m_pixelConstantBuffer);

	context->VSGetShader(&m_pVS, nullptr, nullptr);
	context->PSGetShader(&m_pPS, nullptr, nullptr);

	context->IAGetInputLayout(&m_layout);

	context->IAGetPrimitiveTopology(&m_topology);

	context->PSGetShaderResources(0, Effekseer::TextureSlotMax, m_psSRVs.data());

	context->IAGetVertexBuffers(0, 1, &m_pVB, &m_vbStrides, &m_vbOffset);
	context->IAGetIndexBuffer(&m_pIB, &m_ibFormat, &m_ibOffset);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void OriginalState::LoadState(ID3D11Device* device, ID3D11DeviceContext* context)
{
	context->PSSetSamplers(0, Effekseer::TextureSlotMax, m_samplers.data());
	context->OMSetBlendState(m_blendState, m_blendFactor.data(), m_blendSampleMask);
	context->OMSetDepthStencilState(m_depthStencilState, m_depthStencilStateRef);
	context->RSSetState(m_pRasterizerState);

	context->VSSetConstantBuffers(0, 1, &m_vertexConstantBuffer);
	context->PSSetConstantBuffers(0, 1, &m_pixelConstantBuffer);

	context->VSSetShader(m_pVS, nullptr, 0);
	context->PSSetShader(m_pPS, nullptr, 0);

	context->IASetInputLayout(m_layout);

	context->IASetPrimitiveTopology(m_topology);

	context->PSSetShaderResources(0, Effekseer::TextureSlotMax, m_psSRVs.data());

	context->IASetVertexBuffers(0, 1, &m_pVB, &m_vbStrides, &m_vbOffset);
	context->IASetIndexBuffer(m_pIB, m_ibFormat, m_ibOffset);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void OriginalState::ReleaseState()
{
	for (size_t i = 0; i < m_samplers.size(); i++)
	{
		ES_SAFE_RELEASE(m_samplers[i]);
	}
	ES_SAFE_RELEASE(m_blendState);

	ES_SAFE_RELEASE(m_depthStencilState);

	ES_SAFE_RELEASE(m_pRasterizerState);

	ES_SAFE_RELEASE(m_vertexConstantBuffer);
	ES_SAFE_RELEASE(m_pixelConstantBuffer);

	ES_SAFE_RELEASE(m_pVS);
	ES_SAFE_RELEASE(m_pPS);

	ES_SAFE_RELEASE(m_layout);

	for (size_t i = 0; i < m_psSRVs.size(); i++)
	{
		ES_SAFE_RELEASE(m_psSRVs[i]);
	}

	ES_SAFE_RELEASE(m_pVB);
	ES_SAFE_RELEASE(m_pIB);
}

RendererRef Renderer::Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
							 int32_t squareMaxCount,
							 D3D11_COMPARISON_FUNC depthFunc,
							 bool isMSAAEnabled)
{
	auto renderer = ::Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount);
	if (renderer->Initialize(graphicsDevice.DownCast<Backend::GraphicsDevice>(), depthFunc, isMSAAEnabled))
	{
		return renderer;
	}
	return nullptr;
}

RendererRef Renderer::Create(
	ID3D11Device* device, ID3D11DeviceContext* context, int32_t squareMaxCount, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled)
{
	auto graphicsDevice = Effekseer::MakeRefPtr<Backend::GraphicsDevice>(device, context);
	return Create(graphicsDevice, squareMaxCount, depthFunc, isMSAAEnabled);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: m_device(nullptr)
	, m_context(nullptr)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_renderState(nullptr)
	, m_restorationOfStates(true)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_state = new OriginalState();
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
	ES_SAFE_DELETE(m_state);
	ES_SAFE_DELETE(m_renderState);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
	if (graphicsDevice_ != nullptr)
	{
		graphicsDevice_->LostDevice();
	}

	for (auto& device : m_deviceObjects)
	{
		device->OnLostDevice();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnResetDevice()
{
	for (auto& device : m_deviceObjects)
	{
		device->OnResetDevice();
	}

	if (graphicsDevice_ != nullptr)
	{
		graphicsDevice_->ResetDevice();
	}
}

bool RendererImplemented::Initialize(Backend::GraphicsDeviceRef graphicsDevice,
									 D3D11_COMPARISON_FUNC depthFunc,
									 bool isMSAAEnabled)
{
	graphicsDevice_ = graphicsDevice;
	m_device = graphicsDevice->GetDevice();
	m_context = graphicsDevice->GetContext();
	m_depthFunc = depthFunc;

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

	m_renderState = new RenderState(this, m_depthFunc, isMSAAEnabled);

	auto vlUnlit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Unlit).DownCast<Backend::VertexLayout>();
	auto vlLit = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::Lit).DownCast<Backend::VertexLayout>();
	auto vlDist = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::BackDistortion).DownCast<Backend::VertexLayout>();
	auto vlUnlitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedUnlit).DownCast<Backend::VertexLayout>();
	auto vlLitAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedLit).DownCast<Backend::VertexLayout>();
	auto vlDistAd = EffekseerRenderer::GetVertexLayout(graphicsDevice_, EffekseerRenderer::RendererShaderType::AdvancedBackDistortion).DownCast<Backend::VertexLayout>();

	auto shader_unlit = Shader::Create(graphicsDevice_,
									   graphicsDevice_->CreateShaderFromBinary(
										   Standard_VS::g_main,
										   sizeof(Standard_VS::g_main),
										   Standard_PS::g_main,
										   sizeof(Standard_PS::g_main)),
									   vlUnlit,
									   "Unlit");
	if (shader_unlit == nullptr)
		return false;
	GetImpl()->ShaderUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_unlit);

	auto shader_ad_unlit = Shader::Create(graphicsDevice_,
										  graphicsDevice_->CreateShaderFromBinary(
											  Standard_VS_Ad::g_main,
											  sizeof(Standard_VS_Ad::g_main),
											  Standard_PS_Ad::g_main,
											  sizeof(Standard_PS_Ad::g_main)),
										  vlUnlitAd,
										  "Unlit Ad");
	if (shader_ad_unlit == nullptr)
		return false;
	GetImpl()->ShaderAdUnlit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_unlit);

	auto shader_distortion = Shader::Create(graphicsDevice_,
											graphicsDevice_->CreateShaderFromBinary(
												Standard_Distortion_VS::g_main,
												sizeof(Standard_Distortion_VS::g_main),
												Standard_Distortion_PS::g_main,
												sizeof(Standard_Distortion_PS::g_main)),
											vlDist,
											"Distortion");
	if (shader_distortion == nullptr)
		return false;
	GetImpl()->ShaderDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_distortion);

	auto shader_ad_distortion = Shader::Create(graphicsDevice_,
											   graphicsDevice_->CreateShaderFromBinary(
												   Standard_Distortion_VS_Ad::g_main,
												   sizeof(Standard_Distortion_VS_Ad::g_main),
												   Standard_Distortion_PS_Ad::g_main,
												   sizeof(Standard_Distortion_PS_Ad::g_main)),
											   vlDistAd,
											   "Distortion Ad");
	if (shader_ad_distortion == nullptr)
		return false;
	GetImpl()->ShaderAdDistortion = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_distortion);

	auto shader_lit = Shader::Create(graphicsDevice_,
									 graphicsDevice_->CreateShaderFromBinary(
										 Standard_Lighting_VS::g_main,
										 sizeof(Standard_Lighting_VS::g_main),
										 Standard_Lighting_PS::g_main,
										 sizeof(Standard_Lighting_PS::g_main)),
									 vlLit,
									 "Lit");
	if (shader_lit == nullptr)
		return false;
	GetImpl()->ShaderLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_lit);

	auto shader_ad_lit = Shader::Create(graphicsDevice_,
										graphicsDevice_->CreateShaderFromBinary(
											Standard_Lighting_VS_Ad::g_main,
											sizeof(Standard_Lighting_VS_Ad::g_main),
											Standard_Lighting_PS_Ad::g_main,
											sizeof(Standard_Lighting_PS_Ad::g_main)),
										vlLitAd,
										"Lit Ad");
	if (shader_ad_lit == nullptr)
		return false;
	GetImpl()->ShaderAdLit = std::unique_ptr<EffekseerRenderer::ShaderBase>(shader_ad_lit);

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
	GetImpl()->isSoftParticleEnabled = true;

	GetImpl()->isDepthReversed = !(depthFunc == D3D11_COMPARISON_LESS || depthFunc == D3D11_COMPARISON_LESS_EQUAL);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	m_restorationOfStates = flag;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	assert(m_device != nullptr);

	impl->CalculateCameraProjectionMatrix();

	if (m_restorationOfStates)
	{
		m_state->SaveState(m_device, m_context);
	}

	// reset states
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	// reset a renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	assert(m_device != nullptr);

	// reset a renderer
	m_standardRenderer->ResetAndRenderingIfRequired();

	// restore states
	if (m_restorationOfStates)
	{
		m_state->LoadState(m_device, m_context);
		m_state->ReleaseState();
	}

	currentndexBuffer_ = nullptr;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ID3D11Device* RendererImplemented::GetDevice()
{
	return m_device;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ID3D11DeviceContext* RendererImplemented::GetContext()
{
	return m_context;
}

Effekseer::Backend::IndexBufferRef RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return indexBufferForWireframe_;
	}
	return indexBuffer_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this));
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
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this));
}

::Effekseer::GpuTimerRef RendererImplemented::CreateGpuTimer()
{
	return ::Effekseer::GpuTimerRef(new ::EffekseerRendererDX11::GpuTimer(this, true));
}

::Effekseer::GpuParticleSystemRef RendererImplemented::CreateGpuParticleSystem(const Effekseer::GpuParticleSystem::Settings& settings)
{
	auto gpuParticleSystem = ::Effekseer::GpuParticleSystemRef(new ::EffekseerRendererDX11::GpuParticleSystem(this, true));
	if (!gpuParticleSystem->InitSystem(settings))
	{
		return nullptr;
	}
	return gpuParticleSystem;
}

::Effekseer::GpuParticleFactoryRef RendererImplemented::CreateGpuParticleFactory()
{
	return ::Effekseer::GpuParticleFactoryRef(new ::EffekseerRenderer::GpuParticleFactory(GetGraphicsDevice()));
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

void RendererImplemented::SetBackground(ID3D11ShaderResourceView* background)
{
	if (m_backgroundDX11 == nullptr)
	{
		m_backgroundDX11 = graphicsDevice_->CreateTexture(background, nullptr, nullptr);
	}
	else
	{
		auto texture = static_cast<Backend::Texture*>(m_backgroundDX11.Get());
		texture->Init(background, nullptr, nullptr);
	}

	EffekseerRenderer::Renderer::SetBackground((background) ? m_backgroundDX11 : nullptr);
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(ID3D11Buffer* vertexBuffer, int32_t size)
{
	ID3D11Buffer* vBuf = vertexBuffer;
	uint32_t vertexSize = size;
	uint32_t offset = 0;
	GetContext()->IASetVertexBuffers(0, 1, &vBuf, &vertexSize, &offset);
}

void RendererImplemented::SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size)
{
	auto ptr = static_cast<Backend::VertexBuffer*>(vertexBuffer.Get());
	SetVertexBuffer(ptr->GetBuffer(), size);
}

void RendererImplemented::SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer)
{
	auto ptr = static_cast<Backend::IndexBuffer*>(indexBuffer.Get());
	currentndexBuffer_ = indexBuffer;
	GetContext()->IASetIndexBuffer(ptr->GetBuffer(), ptr->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	if (GetRenderMode() == Effekseer::RenderMode::Normal)
	{
		GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	GetContext()->IASetInputLayout(shader->GetLayoutInterface());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;

	if (GetRenderMode() == Effekseer::RenderMode::Normal)
	{
		GetContext()->DrawIndexed(spriteCount * 2 * 3, 0, vertexOffset);
	}
	else
	{
		GetContext()->DrawIndexed(spriteCount * 2 * 4, 0, vertexOffset);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;

	GetContext()->DrawIndexed(indexCount, 0, 0);
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;
	GetContext()->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}

void RendererImplemented::BeginShader(Shader* shader)
{
	currentShader = shader;
	GetContext()->VSSetShader(shader->GetVertexShader(), nullptr, 0);
	GetContext()->PSSetShader(shader->GetPixelShader(), nullptr, 0);
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
	assert(currentShader->GetVertexConstantBufferSize() >= size + dstOffset);

	auto p = static_cast<uint8_t*>(currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(currentShader != nullptr);
	assert(currentShader->GetPixelConstantBufferSize() >= size + dstOffset);

	auto p = static_cast<uint8_t*>(currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, ::Effekseer::Backend::TextureRef* textures, int32_t count)
{
	std::array<ID3D11ShaderResourceView*, Effekseer::TextureSlotMax> srv;
	for (int32_t i = 0; i < count; i++)
	{
		if (textures[i] == nullptr)
		{
			srv[i] = nullptr;
		}
		else
		{
			auto texture = static_cast<Backend::Texture*>(textures[i].Get());
			srv[i] = texture->GetSRV();
		}
	}

	GetContext()->VSSetShaderResources(0, count, srv.data());
	GetContext()->PSSetShaderResources(0, count, srv.data());
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

Effekseer::Backend::GraphicsDeviceRef RendererImplemented::GetGraphicsDevice() const
{
	return graphicsDevice_;
}

void RendererImplemented::ResetStateForDefferedContext()
{
	GetImpl()->InternalVertexBuffer->GetCurrentBuffer().DownCast<Backend::VertexBuffer>()->MakeAllDirtied();
}

} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
