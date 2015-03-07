
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
#include "EffekseerRendererDX9.SpriteRenderer.h"
#include "EffekseerRendererDX9.RibbonRenderer.h"
#include "EffekseerRendererDX9.RingRenderer.h"
#include "EffekseerRendererDX9.ModelRenderer.h"
#include "EffekseerRendererDX9.TrackRenderer.h"
#include "EffekseerRendererDX9.TextureLoader.h"
#include "EffekseerRendererDX9.ModelLoader.h"

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
	: m_reference	( 1 )
	, m_d3d_device	( NULL )
	, m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_coordinateSystem	( ::Effekseer::CoordinateSystem::RH )
	, m_state_vertexShader( NULL )
	, m_state_pixelShader( NULL )
	, m_state_vertexDeclaration	( NULL )
	, m_state_streamData ( NULL )
	, m_state_IndexData	( NULL )
	, m_state_pTexture	( NULL )
	, m_renderState		( NULL )
	, m_isChangedDevice	( false )
	, m_restorationOfStates( true )
{
	SetLightDirection( ::Effekseer::Vector3D( 1.0f, 1.0f, 1.0f ) );
	SetLightColor( ::Effekseer::Color( 255, 255, 255, 255 ) );
	SetLightAmbientColor( ::Effekseer::Color( 0, 0, 0, 0 ) );

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

	assert( m_reference == 0 );

	ES_SAFE_DELETE(m_standardRenderer);
	ES_SAFE_DELETE(m_shader);
	ES_SAFE_DELETE(m_shader_no_texture);

	ES_SAFE_DELETE( m_renderState );
	ES_SAFE_DELETE( m_vertexBuffer );
	ES_SAFE_DELETE( m_indexBuffer );
	
	//ES_SAFE_RELEASE( m_d3d_device );

	assert( m_reference == -2 );
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

		m_isChangedDevice = false;
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

	// 参照カウントの調整
	Release();

	m_renderState = new RenderState( this );


	// 座標(3) 色(1) UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	//ID3DXBuffer* buf = NULL;

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

	m_shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	m_shader->SetVertexRegisterCount(4);
	m_shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	m_shader_no_texture->SetVertexRegisterCount(4);

	m_standardRenderer = new EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, IDirect3DTexture9*, VertexBuffer>(this, m_shader, m_shader_no_texture);

	//ES_SAFE_ADDREF( m_d3d_device );
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destory()
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

		GetDevice()->GetRenderState( D3DRS_ZENABLE, &m_state_D3DRS_ZENABLE );
		GetDevice()->GetRenderState( D3DRS_ZWRITEENABLE, &m_state_D3DRS_ZWRITEENABLE );
		GetDevice()->GetRenderState( D3DRS_ALPHATESTENABLE, &m_state_D3DRS_ALPHATESTENABLE );
		GetDevice()->GetRenderState( D3DRS_CULLMODE, &m_state_D3DRS_CULLMODE );

		GetDevice()->GetRenderState( D3DRS_COLORVERTEX, &m_state_D3DRS_COLORVERTEX );
		GetDevice()->GetRenderState( D3DRS_LIGHTING, &m_state_D3DRS_LIGHTING );
		GetDevice()->GetRenderState( D3DRS_SHADEMODE, &m_state_D3DRS_SHADEMODE );

		GetDevice()->GetVertexShader(&m_state_vertexShader);
		GetDevice()->GetPixelShader(&m_state_pixelShader);
		GetDevice()->GetVertexDeclaration( &m_state_vertexDeclaration );
		//GetDevice()->GetStreamSource( 0, &m_state_streamData, &m_state_OffsetInBytes, &m_state_pStride );
		//GetDevice()->GetIndices( &m_state_IndexData );
		
			
		GetDevice()->GetTexture( 0, &m_state_pTexture );
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

		GetDevice()->SetRenderState( D3DRS_ZENABLE, m_state_D3DRS_ZENABLE );
		GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, m_state_D3DRS_ZWRITEENABLE );
		GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, m_state_D3DRS_ALPHATESTENABLE );
		GetDevice()->SetRenderState( D3DRS_CULLMODE, m_state_D3DRS_CULLMODE );

		GetDevice()->SetRenderState( D3DRS_COLORVERTEX, m_state_D3DRS_COLORVERTEX );
		GetDevice()->SetRenderState( D3DRS_LIGHTING, m_state_D3DRS_LIGHTING );
		GetDevice()->SetRenderState( D3DRS_SHADEMODE, m_state_D3DRS_SHADEMODE );

		GetDevice()->SetVertexShader(m_state_vertexShader);
		ES_SAFE_RELEASE( m_state_vertexShader );

		GetDevice()->SetPixelShader(m_state_pixelShader);
		ES_SAFE_RELEASE( m_state_pixelShader );

		GetDevice()->SetVertexDeclaration( m_state_vertexDeclaration );
		ES_SAFE_RELEASE( m_state_vertexDeclaration );

		//GetDevice()->SetStreamSource( 0, m_state_streamData, m_state_OffsetInBytes, m_state_pStride );
		//ES_SAFE_RELEASE( m_state_streamData );

		//GetDevice()->SetIndices( m_state_IndexData );
		//ES_SAFE_RELEASE( m_state_IndexData );

		GetDevice()->SetTexture( 0, m_state_pTexture );
		ES_SAFE_RELEASE( m_state_pTexture );

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
	return m_indexBuffer;
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
void RendererImplemented::SetLightDirection( ::Effekseer::Vector3D& direction )
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
void RendererImplemented::SetLightColor( ::Effekseer::Color& color )
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
void RendererImplemented::SetLightAmbientColor( ::Effekseer::Color& color )
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
	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return SpriteRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return RibbonRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return RingRenderer::Create( this );
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
	return TrackRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader( this, fileInterface );
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
	return new ModelLoader( this, fileInterface );
#else
	return NULL;
#endif
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
	GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertexOffset, 0, spriteCount * 4, 0, spriteCount * 2 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon( int32_t vertexCount, int32_t indexCount)
{
	GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertexCount, 0, indexCount / 3 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GetDevice()->SetVertexShader(shader->GetVertexShader());
	GetDevice()->SetPixelShader(shader->GetPixelShader());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, IDirect3DTexture9** textures, int32_t count)
{
	for (int32_t i = 0; i < count; i++)
	{
		GetDevice()->SetTexture(i, textures[i]);
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
