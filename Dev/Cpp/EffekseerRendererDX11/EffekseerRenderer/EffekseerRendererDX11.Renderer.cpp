
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Renderer.h"
#include "EffekseerRendererDX11.RenderState.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.IndexBuffer.h"
#include "EffekseerRendererDX11.Shader.h"
#include "EffekseerRendererDX11.VertexBuffer.h"
//#include "EffekseerRendererDX11.SpriteRenderer.h"
//#include "EffekseerRendererDX11.RibbonRenderer.h"
//#include "EffekseerRendererDX11.RingRenderer.h"
#include "EffekseerRendererDX11.ModelRenderer.h"
//#include "EffekseerRendererDX11.TrackRenderer.h"
#include "EffekseerRendererDX11.MaterialLoader.h"
#include "EffekseerRendererDX11.ModelLoader.h"
#include "EffekseerRendererDX11.TextureLoader.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{

#ifdef __EFFEKSEER_BUILD_VERSION16__
namespace Standard_VS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
} // namespace Standard_VS

namespace Standard_PS
{
static
#include "Shader/EffekseerRenderer.Standard_PS.h"
} // namespace Standard_PS

namespace Standard_Distortion_VS
{
static
#include "Shader/EffekseerRenderer.Standard_Distortion_VS.h"
} // namespace Standard_Distortion_VS

namespace Standard_Distortion_PS
{
static
#include "Shader/EffekseerRenderer.Standard_Distortion_PS.h"
} // namespace Standard_Distortion_PS

namespace Standard_Lighting_VS
{
static
#include "Shader/EffekseerRenderer.Standard_Lighting_VS.h"
} // namespace Standard_Lighting_VS

namespace Standard_Lighting_PS
{
static
#include "Shader/EffekseerRenderer.Standard_Lighting_PS.h"
} // namespace Standard_Lighting_PS

#else

namespace Standard_VS
{
static
#include "Shader_15/EffekseerRenderer.Standard_VS.h"
} // namespace Standard_VS

namespace Standard_PS
{
static
#include "Shader_15/EffekseerRenderer.Standard_PS.h"
} // namespace Standard_PS

namespace Standard_Distortion_VS
{
static
#include "Shader_15/EffekseerRenderer.Standard_Distortion_VS.h"
} // namespace Standard_Distortion_VS

namespace Standard_Distortion_PS
{
static
#include "Shader_15/EffekseerRenderer.Standard_Distortion_PS.h"
} // namespace Standard_Distortion_PS

namespace Standard_Lighting_VS
{
static
#include "Shader_15/EffekseerRenderer.Standard_Lighting_VS.h"
} // namespace Standard_Lighting_VS

namespace Standard_Lighting_PS
{
static
#include "Shader_15/EffekseerRenderer.Standard_Lighting_PS.h"
} // namespace Standard_Lighting_PS

#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
::Effekseer::TextureLoader* CreateTextureLoader(ID3D11Device* device,
												ID3D11DeviceContext* context,
												::Effekseer::FileInterface* fileInterface,
												::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(device, context, fileInterface, colorSpaceType);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* CreateModelLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(device, fileInterface);
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
OriginalState::OriginalState()
	: m_blendState(NULL)
	, m_depthStencilState(NULL)
	, m_depthStencilStateRef(0)
	, m_vertexConstantBuffer(NULL)
	, m_pixelConstantBuffer(NULL)
	, m_layout(NULL)

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

	context->VSSetShader(m_pVS, NULL, 0);
	context->PSSetShader(m_pPS, NULL, 0);

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer* Renderer::Create(
	ID3D11Device* device, ID3D11DeviceContext* context, int32_t squareMaxCount, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled)
{
	RendererImplemented* renderer = new RendererImplemented(squareMaxCount);
	if (renderer->Initialize(device, context, depthFunc, isMSAAEnabled))
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: m_device(NULL)
	, m_context(NULL)
	, m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_renderState(NULL)
	, m_restorationOfStates(true)

	, m_shader(nullptr)
	, m_shader_distortion(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_background.UserPtr = nullptr;

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

	auto p = (ID3D11ShaderResourceView*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);

	ES_SAFE_DELETE(m_shader_distortion);
	ES_SAFE_DELETE(m_shader_lighting);

	ES_SAFE_DELETE(m_state);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	assert(GetRef() == -6);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize(ID3D11Device* device,
									 ID3D11DeviceContext* context,
									 D3D11_COMPARISON_FUNC depthFunc,
									 bool isMSAAEnabled)
{
	m_device = device;
	m_context = context;
	m_depthFunc = depthFunc;

	// generate a vertex buffer
	{
		m_vertexBuffer = VertexBuffer::Create(this, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true);
		if (m_vertexBuffer == NULL)
			return false;
	}

	// 参照カウントの調整
	Release();

	// generate an index buffer
	{
		m_indexBuffer = IndexBuffer::Create(this, m_squareMaxCount * 6, false);
		if (m_indexBuffer == NULL)
			return false;

		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for (int i = 0; i < m_squareMaxCount; i++)
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect(6);
			buf[0] = 3 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 0 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
		}

		m_indexBuffer->Unlock();
	}

	// 参照カウントの調整
	Release();

	// Generate index buffer for rendering wireframes
	{
		m_indexBufferForWireframe = IndexBuffer::Create(this, m_squareMaxCount * 8, false);
		if (m_indexBufferForWireframe == NULL)
			return false;

		m_indexBufferForWireframe->Lock();

		for (int i = 0; i < m_squareMaxCount; i++)
		{
			uint16_t* buf = (uint16_t*)m_indexBufferForWireframe->GetBufferDirect(8);
			buf[0] = 0 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 2 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
			buf[6] = 1 + 4 * i;
			buf[7] = 3 + 4 * i;
		}

		m_indexBufferForWireframe->Unlock();
	}

	// 参照カウントの調整
	Release();

	m_renderState = new RenderState(this, m_depthFunc, isMSAAEnabled);

	// シェーダー
	// 座標(3) 色(1) UV(2)
	D3D11_INPUT_ELEMENT_DESC decl[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
#ifdef __EFFEKSEER_BUILD_VERSION16__
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0}, // AlphaTextureUV
		{"TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0},	 // FlipbookIndexAndNextRate
		{"TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},	 // AlphaThreshold
#endif
	};

	D3D11_INPUT_ELEMENT_DESC decl_distortion[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 9, D3D11_INPUT_PER_VERTEX_DATA, 0},
#ifdef __EFFEKSEER_BUILD_VERSION16__
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0}, // AlphaTextureUV
		{"TEXCOORD", 2, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 14, D3D11_INPUT_PER_VERTEX_DATA, 0},	  // FlipbookIndexAndNextRate
		{"TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 15, D3D11_INPUT_PER_VERTEX_DATA, 0},	  // AlphaThreshold
#endif
	};

	D3D11_INPUT_ELEMENT_DESC decl_lighting[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 2, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 5, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
#ifdef __EFFEKSEER_BUILD_VERSION16__
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 10, D3D11_INPUT_PER_VERTEX_DATA, 0}, // AlphaTextureUVs
		{"TEXCOORD", 3, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 12, D3D11_INPUT_PER_VERTEX_DATA, 0},	  // FlipbookIndexAndNextRate
		{"TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT, 0, sizeof(float) * 13, D3D11_INPUT_PER_VERTEX_DATA, 0},	  // AlphaThreshold
#endif

	};

	m_shader = Shader::Create(
		this,
		Standard_VS::g_VS,
		sizeof(Standard_VS::g_VS),
		Standard_PS::g_PS,
		sizeof(Standard_PS::g_PS),
		"StandardRenderer",
		decl,
		ARRAYSIZE(decl));
	if (m_shader == NULL)
		return false;

	// 参照カウントの調整
	Release();

	m_shader_distortion = Shader::Create(
		this,
		Standard_Distortion_VS::g_VS,
		sizeof(Standard_Distortion_VS::g_VS),
		Standard_Distortion_PS::g_PS,
		sizeof(Standard_Distortion_PS::g_PS),
		"StandardRenderer Distortion",
		decl_distortion,
		ARRAYSIZE(decl_distortion));
	if (m_shader_distortion == NULL)
		return false;

	// 参照カウントの調整
	Release();

#ifdef __EFFEKSEER_BUILD_VERSION16__
	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader->SetVertexRegisterCount(8 + 1 + 1);

	m_shader->SetPixelConstantBufferSize(sizeof(float) * 4 * 1);
	m_shader->SetPixelRegisterCount(1);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion->SetVertexRegisterCount(8 + 1 + 1);

	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1 + 1 + 1);
#else
	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader->SetVertexRegisterCount(8 + 1);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader_distortion->SetVertexRegisterCount(8 + 1);

	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1 + 1);
#endif

	m_shader_lighting = Shader::Create(this,
									   Standard_Lighting_VS::g_VS,
									   sizeof(Standard_Lighting_VS::g_VS),
									   Standard_Lighting_PS::g_PS,
									   sizeof(Standard_Lighting_PS::g_PS),
									   "StandardRenderer Lighting",
									   decl_lighting,
									   ARRAYSIZE(decl_lighting));
	if (m_shader_lighting == NULL)
		return false;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	m_shader_lighting->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_lighting->SetVertexRegisterCount(8 + 1 + 1);

	m_shader_lighting->SetPixelConstantBufferSize(sizeof(float) * 4 * 4);
	m_shader_lighting->SetPixelRegisterCount(4);
#else
	m_shader_lighting->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_lighting->SetVertexRegisterCount(8 + 1 + 1);

	m_shader_lighting->SetPixelConstantBufferSize(sizeof(float) * 4 * 3);
	m_shader_lighting->SetPixelRegisterCount(12);
#endif

	Release();

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(
		this, m_shader, m_shader_distortion);

	GetImpl()->CreateProxyTextures(this);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destroy()
{
	Release();
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
	assert(m_device != NULL);

	impl->CalculateCameraProjectionMatrix();

	// ステートを保存する
	if (m_restorationOfStates)
	{
		m_state->SaveState(m_device, m_context);
	}

	// ステート初期設定
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	assert(m_device != NULL);

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	// ステートを復元する
	if (m_restorationOfStates)
	{
		m_state->LoadState(m_device, m_context);
		m_state->ReleaseState();
	}

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return m_indexBufferForWireframe;
	}
	else
	{
		return m_indexBuffer;
	}
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
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return new ::EffekseerRenderer::RingRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, Vertex, VertexDistortion>(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(this->GetDevice(), this->GetContext(), fileInterface);
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(this->GetDevice(), fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new MaterialLoader(this, fileInterface);
#else
	return nullptr;
#endif
}

Effekseer::TextureData* RendererImplemented::GetBackground()
{
	if (m_background.UserPtr == nullptr)
		return nullptr;
	return &m_background;
}

void RendererImplemented::SetBackground(ID3D11ShaderResourceView* background)
{
	ES_SAFE_ADDREF(background);

	auto p = (ID3D11ShaderResourceView*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);
	m_background.UserPtr = background;
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
{
	ID3D11Buffer* vBuf = vertexBuffer->GetInterface();
	uint32_t vertexSize = size;
	uint32_t offset = 0;
	GetContext()->IASetVertexBuffers(0, 1, &vBuf, &vertexSize, &offset);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	GetContext()->IASetIndexBuffer(indexBuffer->GetInterface(), DXGI_FORMAT_R16_UINT, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(ID3D11Buffer* indexBuffer)
{
	GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
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
		GetContext()->DrawIndexed(
			spriteCount * 2 * 3,
			0,
			vertexOffset);
	}
	else
	{
		GetContext()->DrawIndexed(
			spriteCount * 2 * 4,
			0,
			vertexOffset);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;

	GetContext()->DrawIndexed(
		indexCount,
		0,
		0);
}

Shader* RendererImplemented::GetShader(bool useTexture, ::Effekseer::RendererMaterialType materialType) const
{
	if (materialType == ::Effekseer::RendererMaterialType::BackDistortion)
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader_distortion;
		}
		else
		{
			return m_shader_distortion;
		}
	}
	else if (materialType == ::Effekseer::RendererMaterialType::Lighting)
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader_lighting;
		}
		else
		{
			return m_shader_lighting;
		}
	}
	else
	{
		if (useTexture && GetRenderMode() == Effekseer::RenderMode::Normal)
		{
			return m_shader;
		}
		else
		{
			return m_shader;
		}
	}
}

void RendererImplemented::BeginShader(Shader* shader)
{
	currentShader = shader;
	GetContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
	GetContext()->PSSetShader(shader->GetPixelShader(), NULL, 0);
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
void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
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
			srv[i] = (ID3D11ShaderResourceView*)textures[i]->UserPtr;
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

Effekseer::TextureData* RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	std::array<uint8_t, 4> buf;

	if (type == EffekseerRenderer::ProxyTextureType::White)
	{
		buf.fill(255);
	}
	else if (type == EffekseerRenderer::ProxyTextureType::Normal)
	{
		buf.fill(127);
		buf[2] = 255;
		buf[3] = 255;
	}
	else
	{
		assert(0);
	}

	ID3D11Texture2D* tex = nullptr;

	D3D11_TEXTURE2D_DESC TexDesc{};
	TexDesc.Width = 1;
	TexDesc.Height = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = buf.data();
	data.SysMemPitch = TexDesc.Width * 4;
	data.SysMemSlicePitch = TexDesc.Width * TexDesc.Height * 4;

	HRESULT hr = GetDevice()->CreateTexture2D(&TexDesc, nullptr, &tex);

	if (FAILED(hr))
	{
		return nullptr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = TexDesc.Format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = -1;

	ID3D11ShaderResourceView* texture = nullptr;
	hr = GetDevice()->CreateShaderResourceView(tex, &desc, &texture);
	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(texture);
		return nullptr;
	}

	GetContext()->UpdateSubresource(tex, 0, 0, buf.data(), data.SysMemPitch, 0);

	ES_SAFE_RELEASE(tex);

	// Generate mipmap
	GetContext()->GenerateMips(texture);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = TexDesc.Width;
	textureData->Height = TexDesc.Height;

	return textureData;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
	if (data != nullptr && data->UserPtr != nullptr)
	{
		auto texture = (ID3D11ShaderResourceView*)data->UserPtr;
		texture->Release();
	}

	if (data != nullptr)
	{
		delete data;
	}
}

bool Model::LoadToGPU()
{
	if (IsLoadedOnGPU)
	{
		return false;
	}

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		InternalModels[f].VertexCount = GetVertexCount(f);

		{
			ES_SAFE_RELEASE(InternalModels[f].VertexBuffer);

			std::vector<Effekseer::Model::VertexWithIndex> vs;
			for (int32_t m = 0; m < ModelCount; m++)
			{
				for (int32_t i = 0; i < GetVertexCount(f); i++)
				{
					Effekseer::Model::VertexWithIndex v;
					v.Position = GetVertexes(f)[i].Position;
					v.Normal = GetVertexes(f)[i].Normal;
					v.Binormal = GetVertexes(f)[i].Binormal;
					v.Tangent = GetVertexes(f)[i].Tangent;
					v.UV = GetVertexes(f)[i].UV;
					v.VColor = GetVertexes(f)[i].VColor;
					v.Index[0] = m;

					vs.push_back(v);
				}
			}

			ID3D11Buffer* vb = nullptr;

			D3D11_BUFFER_DESC hBufferDesc;
			hBufferDesc.ByteWidth = sizeof(Effekseer::Model::VertexWithIndex) * GetVertexCount(f) * ModelCount;
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(float);

			D3D11_SUBRESOURCE_DATA hSubResourceData;
			hSubResourceData.pSysMem = &(vs[0]);
			hSubResourceData.SysMemPitch = 0;
			hSubResourceData.SysMemSlicePitch = 0;

			if (FAILED(device_->CreateBuffer(&hBufferDesc, &hSubResourceData, &vb)))
			{
				return false;
			}

			InternalModels[f].VertexBuffer = vb;
		}

		InternalModels[f].FaceCount = GetFaceCount(f);
		InternalModels[f].IndexCount = InternalModels[f].FaceCount * 3;

		{
			ES_SAFE_RELEASE(InternalModels[f].IndexBuffer);

			std::vector<Effekseer::Model::Face> fs;
			for (int32_t m = 0; m < ModelCount; m++)
			{
				for (int32_t i = 0; i < InternalModels[f].FaceCount; i++)
				{
					Effekseer::Model::Face face;
					face.Indexes[0] = GetFaces(f)[i].Indexes[0] + GetVertexCount(f) * m;
					face.Indexes[1] = GetFaces(f)[i].Indexes[1] + GetVertexCount(f) * m;
					face.Indexes[2] = GetFaces(f)[i].Indexes[2] + GetVertexCount(f) * m;
					fs.push_back(face);
				}
			}

			ID3D11Buffer* ib = NULL;
			D3D11_BUFFER_DESC hBufferDesc;
			hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			hBufferDesc.ByteWidth = sizeof(int32_t) * 3 * InternalModels[f].FaceCount * ModelCount;
			hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			hBufferDesc.CPUAccessFlags = 0;
			hBufferDesc.MiscFlags = 0;
			hBufferDesc.StructureByteStride = sizeof(int32_t);

			D3D11_SUBRESOURCE_DATA hSubResourceData;
			hSubResourceData.pSysMem = &(fs[0]);
			hSubResourceData.SysMemPitch = 0;
			hSubResourceData.SysMemSlicePitch = 0;

			if (FAILED(device_->CreateBuffer(&hBufferDesc, &hSubResourceData, &ib)))
			{
				return nullptr;
			}

			InternalModels[f].IndexBuffer = ib;
		}
	}

	IsLoadedOnGPU = true;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
