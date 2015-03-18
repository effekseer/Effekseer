
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
static LPDIRECT3D9			g_d3d = NULL;
static LPDIRECT3DDEVICE9	g_d3d_device = NULL;
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
	
	g_renderer = ::EffekseerRendererDX9::Renderer::Create( g_d3d_device, 2000 );
	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)width / (float)height, 1.0f, 50.0f ) );
	
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
	g_renderer->Destory();
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
	g_d3d_device->BeginScene();
	
	g_renderer->SetLightDirection(::Effekseer::Vector3D(1.0f, 1.0f, 1.0f));
	g_renderer->SetLightAmbientColor(::Effekseer::Color(40, 40, 40, 255));

	g_renderer->BeginRendering();
	g_manager->Draw();
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