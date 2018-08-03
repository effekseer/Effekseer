
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.Renderer.h"
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.Shader.h"
#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.DeviceObject.h"
//#include "EffekseerRendererDX9.SpriteRenderer.h"
//#include "EffekseerRendererDX9.RibbonRenderer.h"
//#include "EffekseerRendererDX9.RingRenderer.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
//#include "EffekseerRendererDX9.TrackRenderer.h"
#include "EffekseerRendererDX9.TextureLoader.h"
#include "EffekseerRendererDX9.ModelLoader.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h"

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
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_PS
{
	static
#include "Shader/EffekseerRenderer.Standard_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace StandardNoTexture_PS
{
	static
#include "Shader/EffekseerRenderer.StandardNoTexture_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_Distortion_VS
{
	static
#include "Shader/EffekseerRenderer.Standard_Distortion_VS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_Distortion_PS
{
	static
#include "Shader/EffekseerRenderer.Standard_Distortion_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace StandardNoTexture_Distortion_PS
{
	static
#include "Shader/EffekseerRenderer.StandardNoTexture_Distortion_PS.h"
}

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
Renderer* Renderer::Create( LPDIRECT3DDEVICE9 device, int32_t squareMaxCount )
{
	RendererImplemented* renderer = new RendererImplemented( squareMaxCount );
	if( renderer->Initialize( device ) )
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented( int32_t squareMaxCount )
	: m_d3d_device	( NULL )
	, m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_coordinateSystem	( ::Effekseer::CoordinateSystem::RH )
	, m_renderMode	( ::Effekseer::RenderMode::Normal )
	, m_state_vertexShader( NULL )
	, m_state_pixelShader( NULL )
	, m_state_vertexDeclaration	( NULL )
	, m_state_streamData ( NULL )
	, m_state_IndexData	( NULL )
	, m_state_pTexture	( {} )
	, m_renderState		( NULL )
	, m_isChangedDevice	( false )
	, m_restorationOfStates( true )

	, m_shader(nullptr)
	, m_shader_no_texture(nullptr)
	, m_shader_distortion(nullptr)
	, m_shader_no_texture_distortion(nullptr)
	, m_standardRenderer(nullptr)
	, m_distortingCallback(nullptr)
{
	m_background.UserPtr = nullptr;

	SetRestorationOfStatesFlag(m_restorationOfStates);
	
	::Effekseer::Vector3D direction( 1.0f, 1.0f, 1.0f );
	SetLightDirection( direction );
	::Effekseer::Color lightColor( 255, 255, 255, 255 );
	SetLightColor( lightColor );
	::Effekseer::Color lightAmbient( 0, 0, 0, 0 );
	SetLightAmbientColor( lightAmbient );

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Finalize();
#endif

	assert(GetRef() == 0);

	ES_SAFE_DELETE(m_distortingCallback);

	auto p = (IDirect3DTexture9*)m_background.UserPtr;
	ES_SAFE_RELEASE(p);

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_no_texture);

	ES_SAFE_DELETE(m_shader_distortion);
	ES_SAFE_DELETE(m_shader_no_texture_distortion);

	ES_SAFE_DELETE( m_renderState );
	ES_SAFE_DELETE( m_vertexBuffer );
	ES_SAFE_DELETE( m_indexBuffer );
	ES_SAFE_DELETE(m_indexBufferForWireframe);

	//ES_SAFE_RELEASE( m_d3d_device );

	assert(GetRef() == -7);
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

	if( m_isChangedDevice )
	{
		// インデックスデータの生成
		GenerateIndexData();

		m_isChangedDevice = false;
	}
}

//----------------------------------------------------------------------------------
// インデックスデータの生成
//----------------------------------------------------------------------------------
void RendererImplemented::GenerateIndexData()
{
	// インデックスの生成
	if( m_indexBuffer != NULL )
	{
		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
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
	if( m_indexBufferForWireframe != NULL )
	{
		m_indexBufferForWireframe->Lock();

		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBufferForWireframe->GetBufferDirect( 8 );
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
bool RendererImplemented::Initialize( LPDIRECT3DDEVICE9 device )
{
	m_d3d_device = device;

	// 頂点の生成
	{
		// 最大でfloat * 10 と仮定
		m_vertexBuffer = VertexBuffer::Create( this, sizeof(float) * 10 * m_squareMaxCount * 4, true );
		if( m_vertexBuffer == NULL ) return false;
	}

	// 参照カウントの調整
	Release();

	// インデックスの生成
	{
		m_indexBuffer = IndexBuffer::Create( this, m_squareMaxCount * 6, false );
		if( m_indexBuffer == NULL ) return false;
	}

	// 参照カウントの調整
	Release();

	// ワイヤーフレーム用インデックスの生成
	{
		m_indexBufferForWireframe = IndexBuffer::Create( this, m_squareMaxCount * 8, false );
		if( m_indexBufferForWireframe == NULL ) return false;
	}

	// 参照カウントの調整
	Release();

	// インデックスデータの生成
	GenerateIndexData();

	m_renderState = new RenderState( this );


	// 座標(3) 色(1) UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_distortion [] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 2 },
		D3DDECL_END()
	};

	m_shader = Shader::Create(
		this,
		Standard_VS::g_vs20_VS,
		sizeof(Standard_VS::g_vs20_VS),
		Standard_PS::g_ps20_PS,
		sizeof(Standard_PS::g_ps20_PS),
		"StandardRenderer", decl);
	if (m_shader == NULL) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture = Shader::Create(
		this,
		Standard_VS::g_vs20_VS,
		sizeof(Standard_VS::g_vs20_VS),
		StandardNoTexture_PS::g_ps20_PS,
		sizeof(StandardNoTexture_PS::g_ps20_PS),
		"StandardRenderer No Texture",
		decl);

	if (m_shader_no_texture == NULL)
	{
		return false;
	}

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
	if (m_shader_distortion == NULL) return false;

	// 参照カウントの調整
	Release();

	m_shader_no_texture_distortion = Shader::Create(
		this,
		Standard_Distortion_VS::g_vs20_VS,
		sizeof(Standard_Distortion_VS::g_vs20_VS),
		StandardNoTexture_Distortion_PS::g_ps20_PS,
		sizeof(StandardNoTexture_Distortion_PS::g_ps20_PS),
		"StandardRenderer No Texture Distortion",
		decl_distortion);

	if (m_shader_no_texture_distortion == NULL)
	{
		return false;
	}

	// 参照カウントの調整
	Release();

	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader->SetVertexRegisterCount(8);
	m_shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader_no_texture->SetVertexRegisterCount(8);

	m_shader_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader_distortion->SetVertexRegisterCount(8);

	m_shader_distortion->SetPixelConstantBufferSize(sizeof(float) * 4);
	m_shader_distortion->SetPixelRegisterCount(1);

	m_shader_no_texture_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
	m_shader_no_texture_distortion->SetVertexRegisterCount(8);

	m_shader_no_texture_distortion->SetPixelConstantBufferSize(sizeof(float) * 4);
	m_shader_no_texture_distortion->SetPixelRegisterCount(1);

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>(
		this, m_shader, m_shader_no_texture, m_shader_distortion, m_shader_no_texture_distortion);

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
	assert( m_d3d_device != NULL );

	::Effekseer::Matrix44::Mul( m_cameraProj, m_camera, m_proj );
	
	// ステートを保存する
	if(m_restorationOfStates)
	{
		GetDevice()->GetRenderState( D3DRS_ALPHABLENDENABLE, &m_state_D3DRS_ALPHABLENDENABLE );
		GetDevice()->GetRenderState( D3DRS_BLENDOP, &m_state_D3DRS_BLENDOP );
		GetDevice()->GetRenderState( D3DRS_DESTBLEND, &m_state_D3DRS_DESTBLEND );
		GetDevice()->GetRenderState( D3DRS_SRCBLEND, &m_state_D3DRS_SRCBLEND );
		GetDevice()->GetRenderState( D3DRS_ALPHAREF, &m_state_D3DRS_ALPHAREF );

		GetDevice()->GetRenderState(D3DRS_DESTBLENDALPHA, &m_state_D3DRS_DESTBLENDALPHA);
		GetDevice()->GetRenderState(D3DRS_SRCBLENDALPHA, &m_state_D3DRS_SRCBLENDALPHA);
		GetDevice()->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, &m_state_D3DRS_SEPARATEALPHABLENDENABLE);
		GetDevice()->GetRenderState(D3DRS_BLENDOPALPHA, &m_state_D3DRS_BLENDOPALPHA);

		GetDevice()->GetRenderState( D3DRS_ZENABLE, &m_state_D3DRS_ZENABLE );
		GetDevice()->GetRenderState( D3DRS_ZWRITEENABLE, &m_state_D3DRS_ZWRITEENABLE );
		GetDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &m_state_D3DRS_ALPHATESTENABLE );
		GetDevice()->GetRenderState( D3DRS_CULLMODE, &m_state_D3DRS_CULLMODE );

		GetDevice()->GetRenderState( D3DRS_COLORVERTEX, &m_state_D3DRS_COLORVERTEX );
		GetDevice()->GetRenderState( D3DRS_LIGHTING, &m_state_D3DRS_LIGHTING );
		GetDevice()->GetRenderState( D3DRS_SHADEMODE, &m_state_D3DRS_SHADEMODE );

		for (int i = 0; i < static_cast<int>(m_state_D3DSAMP_MAGFILTER.size()); i++)
		{
			GetDevice()->GetSamplerState( i, D3DSAMP_MAGFILTER, &m_state_D3DSAMP_MAGFILTER[i] );
			GetDevice()->GetSamplerState( i, D3DSAMP_MINFILTER, &m_state_D3DSAMP_MINFILTER[i] );
			GetDevice()->GetSamplerState( i, D3DSAMP_MIPFILTER, &m_state_D3DSAMP_MIPFILTER[i] );
			GetDevice()->GetSamplerState( i, D3DSAMP_ADDRESSU, &m_state_D3DSAMP_ADDRESSU[i] );
			GetDevice()->GetSamplerState( i, D3DSAMP_ADDRESSV, &m_state_D3DSAMP_ADDRESSV[i] );
		}

		GetDevice()->GetVertexShader(&m_state_vertexShader);
		GetDevice()->GetPixelShader(&m_state_pixelShader);
		GetDevice()->GetVertexDeclaration( &m_state_vertexDeclaration );
		GetDevice()->GetStreamSource( 0, &m_state_streamData, &m_state_OffsetInBytes, &m_state_pStride );
		GetDevice()->GetIndices( &m_state_IndexData );

		GetDevice()->GetVertexShaderConstantF( 0, m_state_VertexShaderConstantF.data(), static_cast<int>(m_state_VertexShaderConstantF.size()) / 4 );
		GetDevice()->GetVertexShaderConstantF( 0, m_state_PixelShaderConstantF.data(), static_cast<int>(m_state_PixelShaderConstantF.size()) / 4 );

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->GetTexture( i, &m_state_pTexture[i] );
		}
		GetDevice()->GetFVF( &m_state_FVF );
	}

	// ステート初期値設定
	GetDevice()->SetTexture( 0, NULL );
	GetDevice()->SetFVF( 0 );

	GetDevice()->SetRenderState( D3DRS_COLORVERTEX, TRUE);
	GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE);
	GetDevice()->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	
	m_renderState->GetActiveState().Reset();
	m_renderState->Update( true );

	GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	assert( m_d3d_device != NULL );
	
	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	// ステートを復元する
	if(m_restorationOfStates)
	{
		GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, m_state_D3DRS_ALPHABLENDENABLE );
		GetDevice()->SetRenderState( D3DRS_BLENDOP, m_state_D3DRS_BLENDOP );
		GetDevice()->SetRenderState( D3DRS_DESTBLEND, m_state_D3DRS_DESTBLEND );
		GetDevice()->SetRenderState( D3DRS_SRCBLEND, m_state_D3DRS_SRCBLEND );
		GetDevice()->SetRenderState( D3DRS_ALPHAREF, m_state_D3DRS_ALPHAREF );
		
		GetDevice()->SetRenderState(D3DRS_DESTBLENDALPHA, m_state_D3DRS_DESTBLENDALPHA);
		GetDevice()->SetRenderState(D3DRS_SRCBLENDALPHA, m_state_D3DRS_SRCBLENDALPHA);
		GetDevice()->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, m_state_D3DRS_SEPARATEALPHABLENDENABLE);
		GetDevice()->SetRenderState(D3DRS_BLENDOPALPHA, m_state_D3DRS_BLENDOPALPHA);

		GetDevice()->SetRenderState( D3DRS_ZENABLE, m_state_D3DRS_ZENABLE );
		GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, m_state_D3DRS_ZWRITEENABLE );
		GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, m_state_D3DRS_ALPHATESTENABLE );
		GetDevice()->SetRenderState( D3DRS_CULLMODE, m_state_D3DRS_CULLMODE );

		GetDevice()->SetRenderState( D3DRS_COLORVERTEX, m_state_D3DRS_COLORVERTEX );
		GetDevice()->SetRenderState( D3DRS_LIGHTING, m_state_D3DRS_LIGHTING );
		GetDevice()->SetRenderState( D3DRS_SHADEMODE, m_state_D3DRS_SHADEMODE );

		for (int i = 0; i < static_cast<int>(m_state_D3DSAMP_MAGFILTER.size()); i++)
		{
			GetDevice()->SetSamplerState( i, D3DSAMP_MAGFILTER, m_state_D3DSAMP_MAGFILTER[i] );
			GetDevice()->SetSamplerState( i, D3DSAMP_MINFILTER, m_state_D3DSAMP_MINFILTER[i] );
			GetDevice()->SetSamplerState( i, D3DSAMP_MIPFILTER, m_state_D3DSAMP_MIPFILTER[i] );
			GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSU, m_state_D3DSAMP_ADDRESSU[i] );
			GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSV, m_state_D3DSAMP_ADDRESSV[i] );
		}

		GetDevice()->SetVertexShader(m_state_vertexShader);
		ES_SAFE_RELEASE( m_state_vertexShader );

		GetDevice()->SetPixelShader(m_state_pixelShader);
		ES_SAFE_RELEASE( m_state_pixelShader );

		GetDevice()->SetVertexDeclaration( m_state_vertexDeclaration );
		ES_SAFE_RELEASE( m_state_vertexDeclaration );

		GetDevice()->SetStreamSource( 0, m_state_streamData, m_state_OffsetInBytes, m_state_pStride );
		ES_SAFE_RELEASE( m_state_streamData );

		GetDevice()->SetIndices( m_state_IndexData );
		ES_SAFE_RELEASE( m_state_IndexData );

		GetDevice()->SetVertexShaderConstantF( 0, m_state_VertexShaderConstantF.data(), m_state_VertexShaderConstantF.size() / 4 );
		GetDevice()->SetVertexShaderConstantF( 0, m_state_PixelShaderConstantF.data(), m_state_PixelShaderConstantF.size() / 4 );

		for (int i = 0; i < static_cast<int>(m_state_pTexture.size()); i++)
		{
			GetDevice()->SetTexture( i, m_state_pTexture[i] );
			ES_SAFE_RELEASE( m_state_pTexture[i] );
		}

		GetDevice()->SetFVF( m_state_FVF );
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
	if( m_renderMode == ::Effekseer::RenderMode::Wireframe )
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
const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
{
	return m_lightDirection;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightDirection( const ::Effekseer::Vector3D& direction )
{
	m_lightDirection = direction;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightColor() const
{
	return m_lightColor;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightColor( const ::Effekseer::Color& color )
{
	m_lightColor = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
{
	return m_lightAmbient;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightAmbientColor( const ::Effekseer::Color& color )
{
	m_lightAmbient = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
{
	return m_proj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetProjectionMatrix( const ::Effekseer::Matrix44& mat )
{
	m_proj = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
{
	return m_camera;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetCameraMatrix( const ::Effekseer::Matrix44& mat )
{
	m_cameraFrontDirection = ::Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);

	auto localPos = ::Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
	auto f = m_cameraFrontDirection;
	auto r = ::Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
	auto u = ::Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

	m_cameraPosition = r * localPos.X + u * localPos.Y + f * localPos.Z;

	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

::Effekseer::Vector3D RendererImplemented::GetCameraFrontDirection() const
{
	return m_cameraFrontDirection;
}

::Effekseer::Vector3D RendererImplemented::GetCameraPosition() const
{
	return m_cameraPosition;
}

void RendererImplemented::SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)
{
	m_cameraFrontDirection = front;
	m_cameraPosition = position;
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
	return new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, Vertex, VertexDistortion>( this );
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
	return ModelRenderer::Create( this );
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
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader(GetDevice(), fileInterface );
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader(GetDevice(), fileInterface );
#else
	return NULL;
#endif
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
void RendererImplemented::SetVertexBuffer( VertexBuffer* vertexBuffer, int32_t size )
{
	GetDevice()->SetStreamSource( 0, vertexBuffer->GetInterface(), 0, size );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer( IndexBuffer* indexBuffer )
{
	GetDevice()->SetIndices( indexBuffer->GetInterface() );
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
void RendererImplemented::SetLayout( Shader* shader )
{
	GetDevice()->SetVertexDeclaration( shader->GetLayoutInterface() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites( int32_t spriteCount, int32_t vertexOffset )
{
	drawcallCount++;
	drawvertexCount += spriteCount * 4;

	if( m_renderMode == ::Effekseer::RenderMode::Normal )
	{
		GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertexOffset, 0, spriteCount * 4, 0, spriteCount * 2 );
	}
	else if( m_renderMode == ::Effekseer::RenderMode::Wireframe )
	{
		GetDevice()->DrawIndexedPrimitive( D3DPT_LINELIST, vertexOffset, 0, spriteCount * 4, 0, spriteCount * 4 );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon( int32_t vertexCount, int32_t indexCount)
{
	drawcallCount++;
	drawvertexCount += vertexCount;

	GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, indexCount / 3 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Shader* RendererImplemented::GetShader(bool useTexture, bool useDistortion) const
{
	if( useDistortion )
	{
		if( useTexture && m_renderMode == Effekseer::RenderMode::Normal )
		{
			return m_shader_distortion;
		}
		else
		{
			return m_shader_no_texture_distortion;
		}
	}
	else
	{
		if( useTexture && m_renderMode == Effekseer::RenderMode::Normal )
		{
			return m_shader;
		}
		else
		{
			return m_shader_no_texture;
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

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size)
{
	memcpy(currentShader->GetVertexConstantBuffer(), data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size)
{
	memcpy(currentShader->GetPixelConstantBuffer(), data, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	for (int32_t i = 0; i < count; i++)
	{
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
void RendererImplemented::ChangeDevice( LPDIRECT3DDEVICE9 device )
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
	m_renderState->Update( true );
}

int32_t RendererImplemented::GetDrawCallCount() const
{
	return drawcallCount;
}

int32_t RendererImplemented::GetDrawVertexCount() const
{
	return drawvertexCount;
}

void RendererImplemented::ResetDrawCallCount()
{
	drawcallCount = 0;
}

void RendererImplemented::ResetDrawVertexCount()
{
	drawvertexCount = 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
