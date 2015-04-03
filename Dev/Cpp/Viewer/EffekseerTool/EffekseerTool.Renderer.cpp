
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Renderer.h"
#include "EffekseerTool.Grid.h"
#include "EffekseerTool.Guide.h"
#include "EffekseerTool.Culling.h"
#include "EffekseerTool.Paste.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	Renderer::DistortingCallback::DistortingCallback(Renderer* renderer)
		: renderer(renderer)
	{
	
	}

	Renderer::DistortingCallback::~DistortingCallback()
	{
	
	}

	void Renderer::DistortingCallback::OnDistorting()
	{

		auto texture = renderer->ExportBackground();
		renderer->m_renderer->SetBackground(texture);
	}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::GenerateRenderTargets(int32_t width, int32_t height)
{
	ES_SAFE_RELEASE(m_renderTarget);
	ES_SAFE_RELEASE(m_renderTargetTexture);
	ES_SAFE_RELEASE(m_renderTargetDepth);

	ES_SAFE_RELEASE(m_renderEffectBackTarget);
	ES_SAFE_RELEASE(m_renderEffectBackTargetTexture);

	if (width == 0 || height == 0) return;

	HRESULT hr;

	hr = GetDevice()->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderTargetTexture, NULL);
	if (FAILED(hr)) return;

	m_renderTargetTexture->GetSurfaceLevel(0, &m_renderTarget);

	GetDevice()->CreateDepthStencilSurface(width, height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_renderTargetDepth, NULL);

	hr = GetDevice()->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_renderEffectBackTargetTexture, NULL);
	if (FAILED(hr)) return;

	m_renderEffectBackTargetTexture->GetSurfaceLevel(0, &m_renderEffectBackTarget);
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer::Renderer( int32_t squareMaxCount )
	: m_handle		( NULL )
	, m_width		( 0 )
	, m_height		( 0 )
	, m_d3d			( NULL )
	, m_d3d_device	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_projection	( PROJECTION_TYPE_PERSPECTIVE )
	, m_renderer	( NULL )

	, RateOfMagnification	( 1.0f )

	, m_grid	( NULL )
	, m_guide	( NULL )
	, m_culling	( NULL )
	, m_background	( NULL )

	, GuideWidth	( 100 )
	, GuideHeight	( 100 )
	, RendersGuide	( false )

	, IsGridShown	( true )

	, IsGridXYShown	( false )
	, IsGridXZShown	( true )
	, IsGridYZShown	( false )

	, IsRightHand	( true )
	, GridLength	( 2.0f )

	, IsCullingShown	(false)
	, CullingRadius		(0.0f)
	, CullingPosition	()

	, m_recording		( false )
	, m_recordingTarget	( NULL )
	, m_recordingTargetTexture	( NULL )
	, m_recordingDepth	( NULL )
	, m_recordingTempTarget	( NULL )
	, m_recordingTempDepth	( NULL )

	, m_backGroundTexture	( NULL )

	, BackgroundColor		( 0, 0, 0, 255 )
	, GridColor				( 255, 255, 255, 255 )
	, IsBackgroundTranslucent	( false )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer::~Renderer()
{
	assert( !m_recording );

	ES_SAFE_RELEASE( m_backGroundTexture );

	ES_SAFE_RELEASE(m_renderTarget);
	ES_SAFE_RELEASE(m_renderTargetTexture);
	ES_SAFE_RELEASE(m_renderTargetDepth);

	ES_SAFE_RELEASE(m_renderEffectBackTarget);
	ES_SAFE_RELEASE(m_renderEffectBackTargetTexture);


	ES_SAFE_DELETE( m_guide );
	ES_SAFE_DELETE( m_grid );
	ES_SAFE_DELETE( m_culling );

	ES_SAFE_DELETE( m_background );

	if( m_renderer != NULL )
	{
		m_renderer->Destory();
		m_renderer = NULL;
	}

	ES_SAFE_RELEASE( m_d3d_device );
	ES_SAFE_RELEASE( m_d3d );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Initialize( HWND handle, int width, int height )
{
	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = width;
	d3dp.BackBufferHeight = height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if( m_d3d == NULL ) return false;

	D3DDEVTYPE	deviceTypes[4];
	DWORD	flags[4];
	
	deviceTypes[0] = D3DDEVTYPE_HAL;
	flags[0] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	deviceTypes[1] = D3DDEVTYPE_HAL;
	flags[1] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	deviceTypes[2] = D3DDEVTYPE_REF;
	flags[2] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	deviceTypes[3] = D3DDEVTYPE_REF;
	flags[3] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	for( int ind = 0; ind < 4; ind++ )
	{
		hr = m_d3d->CreateDevice( 
			D3DADAPTER_DEFAULT,
			deviceTypes[ind],
			handle,
			flags[ind],
			&d3dp,
			&m_d3d_device );
		if( SUCCEEDED( hr ) ) break;
	}

	if( FAILED( hr ) )
	{
		ES_SAFE_RELEASE( m_d3d_device );
		ES_SAFE_RELEASE( m_d3d );
		return false;
	}

	m_handle = handle;
	m_width = width;
	m_height = height;

	m_renderer = (::EffekseerRendererDX9::RendererImplemented*)::EffekseerRendererDX9::Renderer::Create( m_d3d_device, m_squareMaxCount );
	m_renderer->SetDistortingCallback(new DistortingCallback(this));

	// グリッド生成
	m_grid = ::EffekseerRenderer::Grid::Create( m_renderer );

	// ガイド作成
	m_guide = ::EffekseerRenderer::Guide::Create( m_renderer );

	m_culling = ::EffekseerRenderer::Culling::Create( m_renderer );

	// 背景作成
	m_background = ::EffekseerRenderer::Paste::Create( m_renderer );


	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	GenerateRenderTargets(m_width, m_height);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 Renderer::GetDevice()
{
	return m_d3d_device;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Present()
{
	HRESULT hr;

	hr = m_d3d_device->Present( NULL, NULL, NULL, NULL );

	switch ( hr )
	{
		// ドライバ内部の意味不明なエラー
	case D3DERR_DRIVERINTERNALERROR:
		return false;

		// デバイスロスト
	case D3DERR_DEVICELOST:
		while ( FAILED( hr = m_d3d_device->TestCooperativeLevel() ) )
		{
			switch ( hr )
			{
				// デバイスロスト
			case D3DERR_DEVICELOST:
				::SleepEx( 1000, true );
				break;
				// デバイスロスト：リセット可
			case D3DERR_DEVICENOTRESET:
				ResetDevice();
				break;
			}
		}
		break;
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::ResetDevice()
{
	GenerateRenderTargets(0, 0);

	m_renderer->OnLostDevice();

	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = m_width;
	d3dp.BackBufferHeight = m_height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = m_handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;


	hr = m_d3d_device->Reset( &d3dp );

	if( FAILED( hr ) )
	{
		assert(0);
		return;
	}

	m_renderer->OnResetDevice();

	GenerateRenderTargets(m_width, m_height);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
eProjectionType Renderer::GetProjectionType()
{
	return m_projection;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetProjectionType( eProjectionType type )
{
	m_projection = type;

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( m_width, m_height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( m_width, m_height );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetPerspectiveFov( int width, int height )
{
	::Effekseer::Matrix44 proj;

	if( IsRightHand )
	{
		// 右手
		proj.PerspectiveFovRH( 60.0f / 180.0f * 3.141592f, (float)width / (float)height, 1.0f, 300.0f );
	}
	else
	{
		// 左手
		proj.PerspectiveFovLH( 60.0f / 180.0f * 3.141592f, (float)width / (float)height, 1.0f, 300.0f );
	}

	proj.Values[0][0] *= RateOfMagnification;
	proj.Values[1][1] *= RateOfMagnification;

	m_renderer->SetProjectionMatrix( proj );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetOrthographic( int width, int height )
{
	::Effekseer::Matrix44 proj;

	if( IsRightHand )
	{
		// 右手
		proj.OrthographicRH( (float)width / 16.0f / RateOfMagnification, (float)height / 16.0f / RateOfMagnification, 1.0f, 300.0f );
	}
	else
	{
		// 左手
		proj.OrthographicLH( (float)width / 16.0f / RateOfMagnification, (float)height / 16.0f / RateOfMagnification, 1.0f, 300.0f );
	}

	m_renderer->SetProjectionMatrix( proj );
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::Resize( int width, int height )
{
	m_width = width;
	m_height = height;

	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( width, height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( width, height );
	}

	ResetDevice();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::RecalcProjection()
{
	if( m_projection == PROJECTION_TYPE_PERSPECTIVE )
	{
		SetPerspectiveFov( m_width, m_height );
	}
	else if( m_projection == PROJECTION_TYPE_ORTHOGRAPHIC )
	{
		SetOrthographic( m_width, m_height );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::BeginRendering()
{
	assert( m_d3d_device != NULL );

	HRESULT hr;

	if (!m_recording)
	{
		GetDevice()->GetRenderTarget(0, &m_renderDefaultTarget);
		GetDevice()->GetDepthStencilSurface(&m_renderDefaultDepth);
		
		GetDevice()->SetRenderTarget(0, m_renderTarget);
		GetDevice()->SetDepthStencilSurface(m_renderTargetDepth);

		m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	}

	if( m_recording && IsBackgroundTranslucent )
	{
		m_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0 );
	}
	else
	{
		m_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(BackgroundColor.R,BackgroundColor.G,BackgroundColor.B), 1.0f, 0 );
	}

	// ガイド部分が描画されるように拡大
	if( m_recording )
	{
		::Effekseer::Matrix44 mat;
		mat.Values[0][0] = (float)m_width / (float)GuideWidth;
		mat.Values[1][1] = (float)m_height / (float)GuideHeight;
		::Effekseer::Matrix44::Mul( m_renderer->GetCameraProjectionMatrix(), m_renderer->GetCameraProjectionMatrix(), mat );
	}

	hr = m_d3d_device->BeginScene();

	if( FAILED( hr ) ) return false;

	/* 背景 */
	if( !m_recording && m_backGroundTexture != NULL )
	{
		// 値は適当(背景は画面サイズと一致しないので問題ない)
		m_background->Rendering(m_backGroundTexture, 1024, 1024);
	}

	if( !m_recording && IsGridShown )
	{
		m_grid->SetLength( GridLength );
		m_grid->Rendering( GridColor, IsRightHand );
		m_grid->IsShownXY = IsGridXYShown;
		m_grid->IsShownXZ = IsGridXZShown;
		m_grid->IsShownYZ = IsGridYZShown;
	}

	if( !m_recording )
	{
		m_culling->IsShown = IsCullingShown;
		m_culling->Radius = CullingRadius;
		m_culling->X = CullingPosition.X;
		m_culling->Y = CullingPosition.Y;
		m_culling->Z = CullingPosition.Z;
		m_culling->Rendering( IsRightHand );
	}

	m_renderer->BeginRendering();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::EndRendering()
{
	assert( m_d3d != NULL );
	assert( m_d3d_device != NULL );

	m_renderer->EndRendering();

	if( RendersGuide && !m_recording )
	{
		m_guide->Rendering( m_width, m_height, GuideWidth, GuideHeight );
	}

	if (!m_recording)
	{
		GetDevice()->SetRenderTarget(0, m_renderDefaultTarget);
		GetDevice()->SetDepthStencilSurface(m_renderDefaultDepth);
		ES_SAFE_RELEASE(m_renderDefaultTarget);
		ES_SAFE_RELEASE(m_renderDefaultDepth);
		
		m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		m_background->Rendering(m_renderTargetTexture, m_width, m_height);

		m_renderer->SetBackground(nullptr);
	}
	
	m_d3d_device->EndScene();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IDirect3DTexture9* Renderer::ExportBackground()
{
	if (m_recording) return nullptr;
	if (m_renderDefaultTarget == nullptr) return nullptr;

	GetDevice()->SetRenderTarget(0, m_renderEffectBackTarget);
	GetDevice()->SetDepthStencilSurface(nullptr);
	
	m_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	m_background->Rendering(m_renderTargetTexture, m_width, m_height);

	GetDevice()->SetRenderTarget(0, m_renderTarget);
	GetDevice()->SetDepthStencilSurface(m_renderTargetDepth);

	return m_renderEffectBackTargetTexture;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Renderer::BeginRecord( int32_t width, int32_t height )
{
	assert( !m_recording );
	assert( m_recordingTempTarget == NULL );
	assert( m_recordingTempDepth == NULL );

	HRESULT hr;

	hr = GetDevice()->CreateTexture( width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_recordingTargetTexture, NULL );
	if( FAILED( hr ) ) return false;

	m_recordingTargetTexture->GetSurfaceLevel( 0, &m_recordingTarget );

	GetDevice()->CreateDepthStencilSurface( width, height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_recordingDepth, NULL );

	if( m_recordingTarget == NULL || m_recordingDepth == NULL )
	{
		ES_SAFE_RELEASE( m_recordingTarget );
		ES_SAFE_RELEASE( m_recordingTargetTexture );
		ES_SAFE_RELEASE( m_recordingDepth );
		return false;
	}

	GetDevice()->GetRenderTarget( 0, &m_recordingTempTarget );
	GetDevice()->GetDepthStencilSurface( &m_recordingTempDepth );

	GetDevice()->SetRenderTarget( 0, m_recordingTarget );
	GetDevice()->SetDepthStencilSurface( m_recordingDepth );
	
	m_recording = true;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::SetRecordRect( int32_t x, int32_t y )
{
	assert( m_recording );

	D3DVIEWPORT9 vp;
	vp.X = x;
	vp.Y = y;
	vp.Width = GuideWidth;
	vp.Height = GuideHeight;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
		
	GetDevice()->SetViewport( &vp );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::EndRecord( const wchar_t* outputPath )
{
	assert( m_recording );

	GetDevice()->SetRenderTarget( 0, m_recordingTempTarget );
	GetDevice()->SetDepthStencilSurface( m_recordingTempDepth );
	ES_SAFE_RELEASE( m_recordingTempTarget );
	ES_SAFE_RELEASE( m_recordingTempDepth );

	D3DXSaveSurfaceToFileW( outputPath, D3DXIFF_PNG, m_recordingTarget, NULL, NULL );

	ES_SAFE_RELEASE( m_recordingTarget );
	ES_SAFE_RELEASE( m_recordingTargetTexture );
	ES_SAFE_RELEASE( m_recordingDepth );

	m_recording = false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Renderer::LoadBackgroundImage( void* data, int32_t size )
{
	ES_SAFE_RELEASE( m_backGroundTexture );

	if( data != NULL )
	{
		D3DXCreateTextureFromFileInMemory( m_renderer->GetDevice(), data, size, &m_backGroundTexture );
	}
	else
	{
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
