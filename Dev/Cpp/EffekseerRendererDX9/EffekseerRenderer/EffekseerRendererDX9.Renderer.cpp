
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.Renderer.h"
#include "EffekseerRendererDX9.RenderState.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

#include "EffekseerRendererDX9.DeviceObject.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.Shader.h"
#include "EffekseerRendererDX9.VertexBuffer.h"
//#include "EffekseerRendererDX9.SpriteRenderer.h"
//#include "EffekseerRendererDX9.RibbonRenderer.h"
//#include "EffekseerRendererDX9.RingRenderer.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
//#include "EffekseerRendererDX9.TrackRenderer.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"
#include "EffekseerRendererDX9.MaterialLoader.h"
#include "EffekseerRendererDX9.ModelLoader.h"
#include "EffekseerRendererDX9.TextureLoader.h"

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_VS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
} // namespace Standard_VS

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_PS
{
static
#include "Shader/EffekseerRenderer.Standard_PS.h"
} // namespace Standard_PS

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_Distortion_VS
{
static
#include "Shader/EffekseerRenderer.Standard_Distortion_VS.h"
} // namespace Standard_Distortion_VS

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
::Effekseer::TextureLoader* CreateTextureLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface)
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(device, fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::ModelLoader* CreateModelLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface)
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
Renderer* Renderer::Create(LPDIRECT3DDEVICE9 device, int32_t squareMaxCount)
{
	RendererImplemented* renderer = new RendererImplemented(squareMaxCount);
	if (renderer->Initialize(device))
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: m_d3d_device(NULL)
	, m_vertexBuffer(NULL)
	, m_indexBuffer(NULL)
	, m_squareMaxCount(squareMaxCount)
	, m_coordinateSystem(::Effekseer::CoordinateSystem::RH)
	, m_state_vertexShader(NULL)
	, m_state_pixelShader(NULL)
	, m_state_vertexDeclaration(NULL)
	, m_state_streamData(NULL)
	, m_state_IndexData(NULL)
	, m_state_pTexture({})
	, m_renderState(NULL)
	, m_isChangedDevice(false)
	, m_restorationOfStates(true)

	, m_shader(nullptr)
	, m_shader_distortion(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_background.UserPtr = nullptr;

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

	auto p = (IDirect3DTexture9*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);

	ES_SAFE_DELETE(m_shader_distortion);
	ES_SAFE_DELETE(m_shader_lighting);

	ES_SAFE_DELETE(m_renderState);
	ES_SAFE_DELETE(m_vertexBuffer);
	ES_SAFE_DELETE(m_indexBuffer);
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	//ES_SAFE_RELEASE( m_d3d_device );

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

	GetImpl()->DeleteProxyTextures(this);
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

	if (m_isChangedDevice)
	{
		// インデックスデータの生成
		GenerateIndexData();

		m_isChangedDevice = false;
	}

	GetImpl()->CreateProxyTextures(this);
}

//----------------------------------------------------------------------------------
// インデックスデータの生成
//----------------------------------------------------------------------------------
void RendererImplemented::GenerateIndexData()
{
	// generate an index buffer
	if (m_indexBuffer != NULL)
	{
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

	// Generate index buffer for rendering wireframes
	if (m_indexBufferForWireframe != NULL)
	{
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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize(LPDIRECT3DDEVICE9 device)
{
	m_d3d_device = device;

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
	}

	// 参照カウントの調整
	Release();

	// generate an index buffer for a wireframe
	{
		m_indexBufferForWireframe = IndexBuffer::Create(this, m_squareMaxCount * 8, false);
		if (m_indexBufferForWireframe == NULL)
			return false;
	}

	// 参照カウントの調整
	Release();

	// インデックスデータの生成
	GenerateIndexData();

	m_renderState = new RenderState(this);

	// 座標(3) 色(1) UV(2)
	D3DVERTEXELEMENT9 decl[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
			{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			D3DDECL_END()};

	D3DVERTEXELEMENT9 decl_distortion[] =
		{
			{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
			{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1},
			{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2},
			D3DDECL_END()};

	m_shader = Shader::Create(
		this,
		Standard_VS::g_vs20_VS,
		sizeof(Standard_VS::g_vs20_VS),
		Standard_PS::g_ps20_PS,
		sizeof(Standard_PS::g_ps20_PS),
		"StandardRenderer",
		decl);
	if (m_shader == NULL)
		return false;

	// 参照カウントの調整
	Release();

	m_shader_distortion = Shader::Create(
		this,
		Standard_Distortion_VS::g_vs20_VS,
		sizeof(Standard_Distortion_VS::g_vs20_VS),
		Standard_Distortion_PS::g_ps20_PS,
		sizeof(Standard_Distortion_PS::g_ps20_PS),
		"StandardRenderer Distortion",
		decl_distortion);
	if (m_shader_distortion == NULL)
		return false;

	// 参照カウントの調整
	Release();

	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader->SetVertexRegisterCount(8 + 1);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader_distortion->SetVertexRegisterCount(8 + 1);

	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 + sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1 + 1);

	D3DVERTEXELEMENT9 decl_lighting[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
										 {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
										 {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1},
										 {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2},
										 {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
										 {0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
										 D3DDECL_END()};

	m_shader_lighting = Shader::Create(this,
									   Standard_Lighting_VS::g_vs20_VS,
									   sizeof(Standard_Lighting_VS::g_vs20_VS),
									   Standard_Lighting_PS::g_ps20_PS,
									   sizeof(Standard_Lighting_PS::g_ps20_PS),
									   "StandardRenderer Lighting",
									   decl_lighting);
	if (m_shader_lighting == NULL)
		return false;

	m_shader_lighting->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4);
	m_shader_lighting->SetVertexRegisterCount(8 + 1);

	m_shader_lighting->SetPixelConstantBufferSize(sizeof(float) * 4 * 3);
	m_shader_lighting->SetPixelRegisterCount(12);
	Release();

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(
		this, m_shader, m_shader_distortion);

	GetImpl()->CreateProxyTextures(this);

	//ES_SAFE_ADDREF( m_d3d_device );
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
	assert(m_d3d_device != NULL);

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
		GetDevice()->GetStreamSource(0, &m_state_streamData, &m_state_OffsetInBytes, &m_state_pStride);
		GetDevice()->GetIndices(&m_state_IndexData);

		GetDevice()->GetVertexShaderConstantF(0, m_state_VertexShaderConstantF.data(), static_cast<int>(m_state_VertexShaderConstantF.size()) / 4);
		GetDevice()->GetVertexShaderConstantF(0, m_state_PixelShaderConstantF.data(), static_cast<int>(m_state_PixelShaderConstantF.size()) / 4);

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->GetTexture(i, &m_state_pTexture[i]);
		}
		GetDevice()->GetFVF(&m_state_FVF);
	}

	// ステート初期値設定
	GetDevice()->SetTexture(0, NULL);
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
	assert(m_d3d_device != NULL);

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

		GetDevice()->SetStreamSource(0, m_state_streamData, m_state_OffsetInBytes, m_state_pStride);
		ES_SAFE_RELEASE(m_state_streamData);

		GetDevice()->SetIndices(m_state_IndexData);
		ES_SAFE_RELEASE(m_state_IndexData);

		GetDevice()->SetVertexShaderConstantF(0, m_state_VertexShaderConstantF.data(), m_state_VertexShaderConstantF.size() / 4);
		GetDevice()->SetVertexShaderConstantF(0, m_state_PixelShaderConstantF.data(), m_state_PixelShaderConstantF.size() / 4);

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->SetTexture(i, m_state_pTexture[i]);
			ES_SAFE_RELEASE(m_state_pTexture[i]);
		}

		GetDevice()->SetFVF(m_state_FVF);
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 RendererImplemented::GetDevice()
{
	return m_d3d_device;
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
IndexBuffer* RendererImplemented::GetIndexBufferForWireframe()
{
	return m_indexBufferForWireframe;
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
	return new TextureLoader(this, fileInterface);
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
	return new ModelLoader(this, fileInterface);
#else
	return NULL;
#endif
}

::Effekseer::MaterialLoader* RendererImplemented::CreateMaterialLoader(::Effekseer::FileInterface* fileInterface)
{
	return new MaterialLoader(this, fileInterface);
}

void RendererImplemented::SetBackground(IDirect3DTexture9* background)
{
	ES_SAFE_ADDREF(background);

	auto p = (IDirect3DTexture9*)m_background.UserPtr;
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
void RendererImplemented::SetVertexBuffer(IDirect3DVertexBuffer9* vertexBuffer, int32_t size)
{
	GetDevice()->SetStreamSource(0, vertexBuffer, 0, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size)
{
	GetDevice()->SetStreamSource(0, vertexBuffer->GetInterface(), 0, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(IndexBuffer* indexBuffer)
{
	GetDevice()->SetIndices(indexBuffer->GetInterface());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(IDirect3DIndexBuffer9* indexBuffer)
{
	GetDevice()->SetIndices(indexBuffer);
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

	GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, indexCount / 3);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
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
				GetDevice()->SetTexture(i + D3DVERTEXTEXTURESAMPLER0, (IDirect3DTexture9*)textures[i]->UserPtr);
			}
		}

		if (textures[i] == nullptr)
		{
			GetDevice()->SetTexture(i, nullptr);
		}
		else
		{
			GetDevice()->SetTexture(i, (IDirect3DTexture9*)textures[i]->UserPtr);
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ChangeDevice(LPDIRECT3DDEVICE9 device)
{
	m_d3d_device = device;

	for (auto& device : m_deviceObjects)
	{
		device->OnChangeDevice();
	}

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

	HRESULT hr;
	int32_t width = 1;
	int32_t height = 1;
	int32_t mipMapCount = 1;
	LPDIRECT3DTEXTURE9 texture = nullptr;
	hr = GetDevice()->CreateTexture(width, height, mipMapCount, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);

	if (FAILED(hr))
	{
		return nullptr;
	}

	LPDIRECT3DTEXTURE9 tempTexture = nullptr;
	hr = GetDevice()->CreateTexture(width, height, mipMapCount, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tempTexture, NULL);

	if (FAILED(hr))
	{
		ES_SAFE_RELEASE(texture);
		return nullptr;
	}

	D3DLOCKED_RECT locked;
	if (SUCCEEDED(tempTexture->LockRect(0, &locked, NULL, 0)))
	{
		uint8_t* destBits = (uint8_t*)locked.pBits;
		destBits[0] = buf[2];
		destBits[2] = buf[0];
		destBits[1] = buf[1];
		destBits[3] = buf[3];

		tempTexture->UnlockRect(0);
	}

	hr = GetDevice()->UpdateTexture(tempTexture, texture);
	ES_SAFE_RELEASE(tempTexture);

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = width;
	textureData->Height = height;
	return textureData;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
	if (data != nullptr && data->UserPtr != nullptr)
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data->UserPtr;
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

	// get device
	LPDIRECT3DDEVICE9 device = nullptr;
	if (device_ != nullptr)
	{
		device = device_;
	}

	HRESULT hr;

	for (int32_t f = 0; f < GetFrameCount(); f++)
	{
		InternalModels[f].VertexCount = GetVertexCount(f);

		IDirect3DVertexBuffer9* vb = NULL;
		hr =
			device->CreateVertexBuffer(sizeof(Effekseer::Model::VertexWithIndex) * InternalModels[f].VertexCount * ModelCount,
									   D3DUSAGE_WRITEONLY,
									   0,
									   D3DPOOL_MANAGED,
									   &vb,
									   NULL);

		if (FAILED(hr))
		{
			/* DirectX9ExではD3DPOOL_MANAGED使用不可 */
			hr = device->CreateVertexBuffer(sizeof(Effekseer::Model::VertexWithIndex) * InternalModels[f].VertexCount *
												ModelCount,
											D3DUSAGE_WRITEONLY,
											0,
											D3DPOOL_DEFAULT,
											&vb,
											NULL);
		}

		ES_SAFE_RELEASE(InternalModels[f].VertexBuffer);

		if (vb != NULL)
		{
			uint8_t* resource = NULL;
			vb->Lock(0, 0, (void**)&resource, 0);

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

					std::swap(v.VColor.R, v.VColor.B);

					v.Index[0] = m;

					memcpy(resource, &v, sizeof(Effekseer::Model::VertexWithIndex));
					resource += sizeof(Effekseer::Model::VertexWithIndex);
				}
			}

			vb->Unlock();
		}

		InternalModels[f].VertexBuffer = vb;

		InternalModels[f].FaceCount = GetFaceCount(f);
		InternalModels[f].IndexCount = InternalModels[f].FaceCount * 3;

		IDirect3DIndexBuffer9* ib = NULL;
		hr = device->CreateIndexBuffer(sizeof(Effekseer::Model::Face) * InternalModels[f].FaceCount * ModelCount,
									   D3DUSAGE_WRITEONLY,
									   D3DFMT_INDEX32,
									   D3DPOOL_MANAGED,
									   &ib,
									   NULL);

		if (FAILED(hr))
		{
			hr = device->CreateIndexBuffer(sizeof(Effekseer::Model::Face) * InternalModels[f].FaceCount * ModelCount,
										   D3DUSAGE_WRITEONLY,
										   D3DFMT_INDEX32,
										   D3DPOOL_DEFAULT,
										   &ib,
										   NULL);
		}

		ES_SAFE_RELEASE(InternalModels[f].IndexBuffer);

		if (ib != NULL)
		{
			uint8_t* resource = NULL;
			ib->Lock(0, 0, (void**)&resource, 0);
			for (int32_t m = 0; m < ModelCount; m++)
			{
				for (int32_t i = 0; i < InternalModels[f].FaceCount; i++)
				{
					Effekseer::Model::Face face;
					face.Indexes[0] = GetFaces(f)[i].Indexes[0] + GetVertexCount(f) * m;
					face.Indexes[1] = GetFaces(f)[i].Indexes[1] + GetVertexCount(f) * m;
					face.Indexes[2] = GetFaces(f)[i].Indexes[2] + GetVertexCount(f) * m;

					memcpy(resource, &face, sizeof(Effekseer::Model::Face));
					resource += sizeof(Effekseer::Model::Face);
				}
			}

			ib->Unlock();
		}

		InternalModels[f].IndexBuffer = ib;
	}

	IsLoadedOnGPU = true;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
