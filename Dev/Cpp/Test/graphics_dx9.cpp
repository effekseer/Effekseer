
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <assert.h>
#include "common.h"

#include "../EffekseerRendererDX9/EffekseerRendererDX9.h"
#include "graphics.h"
#include "window.h"

#if _DEBUG
#pragma comment(lib, "x86/EffekseerRendererDX9.Debug.lib" )
#else
#pragma comment(lib, "x86/EffekseerRendererDX9.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
	::EffekseerRendererDX9::Renderer* renderer = nullptr;
	LPDIRECT3DDEVICE9 device = nullptr;
	LPDIRECT3DTEXTURE9 texture = nullptr;

public:
	DistortingCallback( ::EffekseerRendererDX9::Renderer* renderer, 
		LPDIRECT3DDEVICE9 device, int texWidth, int texHeight )
		: renderer( renderer ), device( device )
	{
		device->CreateTexture( texWidth, texHeight, 1, D3DUSAGE_RENDERTARGET, 
			D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL );
	}

	virtual ~DistortingCallback()
	{
		ES_SAFE_RELEASE( texture );
	}

	virtual bool OnDistorting() override
	{
		IDirect3DSurface9* targetSurface = nullptr;
		IDirect3DSurface9* texSurface = nullptr;
		HRESULT hr = S_OK;

		hr = texture->GetSurfaceLevel( 0, &texSurface );
		assert(SUCCEEDED(hr));

		hr = device->GetRenderTarget( 0, &targetSurface );
		assert(SUCCEEDED(hr));

		hr = device->StretchRect( targetSurface, NULL, texSurface, NULL, D3DTEXF_NONE);
		assert(SUCCEEDED(hr));
		
		ES_SAFE_RELEASE( texSurface );
		ES_SAFE_RELEASE( targetSurface );

		renderer->SetBackground( texture );

		return true;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static LPDIRECT3D9			g_d3d = NULL;
static LPDIRECT3DDEVICE9	g_d3d_device = NULL;
static LPDIRECT3DSURFACE9	g_d3d_clearing_image = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitGraphics(int width, int height )
{
	InitWindow(width, height);

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = width;
	d3dp.BackBufferHeight = height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = (HWND)GetHandle();
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

#if __PerformanceCheckMode
	d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	
	g_d3d->CreateDevice( 
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		(HWND) GetHandle(),
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dp,
		&g_d3d_device );

	
	{// 市松模様の背景画像を作る
		g_d3d_device->CreateOffscreenPlainSurface( width, height, 
			D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &g_d3d_clearing_image, NULL );
		D3DLOCKED_RECT lockedRect;
		g_d3d_clearing_image->LockRect( &lockedRect, NULL, 0 );
		CreateCheckeredPattern( width, height, (uint32_t*)lockedRect.pBits );
		g_d3d_clearing_image->UnlockRect();
	}

	g_renderer = ::EffekseerRendererDX9::Renderer::Create( g_d3d_device, 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)width / (float)height, 1.0f, 50.0f ) );
	g_renderer->SetDistortingCallback( new DistortingCallback( 
		(EffekseerRendererDX9::Renderer*)g_renderer, g_d3d_device, width, height ) );
	
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );

	g_manager->SetCoordinateSystem( ::Effekseer::CoordinateSystem::RH );

	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermGraphics()
{
	g_renderer->Destroy();
	
	ES_SAFE_RELEASE( g_d3d_clearing_image );
	ES_SAFE_RELEASE( g_d3d_device );
	ES_SAFE_RELEASE( g_d3d );

	ExitWindow();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Rendering()
{
	g_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
	
	LPDIRECT3DSURFACE9 targetSurface = NULL;
	g_d3d_device->GetRenderTarget( 0, &targetSurface );
	g_d3d_device->UpdateSurface( g_d3d_clearing_image, NULL, targetSurface, NULL );
	ES_SAFE_RELEASE( targetSurface );

	g_d3d_device->BeginScene();
	
	g_renderer->SetLightDirection(::Effekseer::Vector3D(1.0f, 1.0f, 1.0f));
	g_renderer->SetLightAmbientColor(::Effekseer::Color(40, 40, 40, 255));

	g_renderer->BeginRendering();
	//g_manager->Draw();
	g_manager->DrawBack();
	g_manager->DrawFront();
	g_renderer->EndRendering();

	g_d3d_device->EndScene();
	g_d3d_device->Present( NULL, NULL, NULL, NULL );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool DoEvent()
{
	return DoWindowEvent();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SetCameraMatrix( const ::Effekseer::Matrix44& matrix )
{
	g_renderer->SetCameraMatrix( matrix );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------