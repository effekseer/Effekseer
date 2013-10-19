#if 0

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <assert.h>
#include <windows.h>

#include "graphics.h"
#include "sound.h"

#include "../Effekseer/Effekseer.h"

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
//#define __DIRECT 1

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop();
void Init();

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;

static int32_t g_count = 0;

 ::Effekseer::Manager*					g_manager = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::Vector3D			g_focus;

static ::Effekseer::Server*				g_server = NULL;
static ::Effekseer::Client*				g_client = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) 
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitWindow()
{
	WNDCLASS wndClass;
	wchar_t szClassNme[]      =  L"Effekseer";
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = WndProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandle(0);
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = szClassNme;
	RegisterClass(&wndClass);
	g_window_handle = CreateWindow(
		szClassNme,
		L"Effekseer Test Program",
		WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		g_window_width,
		g_window_height,
		NULL,
		NULL,
		GetModuleHandle(0),
		NULL);
	ShowWindow( g_window_handle, true );
	UpdateWindow( g_window_handle );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void MainLoop()
{
	for(;;)
	{ 
		MSG msg;
		if (PeekMessage (&msg,NULL,0,0,PM_NOREMOVE)) 
		{
			if( msg.message == WM_QUIT )
			{
				return ;
			}
			GetMessage (&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Loop();

			Rendering();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	InitWindow();

	g_manager = ::Effekseer::Manager::Create( 2000, false );

	InitGraphics( g_window_handle, g_window_width, g_window_height );

	InitSound( g_window_handle );

	Init();

	MainLoop();

	g_manager->Destroy();

	ES_SAFE_RELEASE( g_effect );

	TermSound();

	TermGraphics();

	ES_SAFE_DELETE( g_server );
	ES_SAFE_DELETE( g_client );

	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Init()
{
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f );
	g_focus = ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f );

	SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( g_position, g_focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"test.efk" );


#ifndef __DIRECT
	g_server = Effekseer::Server::Create();
	g_client = Effekseer::Client::Create();
	g_server->Start( 37565 );
	g_client->Start( "127.0.0.1", 37565 );
	g_server->Regist( (const EFK_CHAR*)L"test", g_effect );
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop()
{
	if( g_count == 0 )
	{
		srand(0);
		g_handle = g_manager->Play( g_effect, 0, 0, 0 );
	}

	if( g_count == 60 )
	{
		srand(0);

#ifdef __DIRECT
		g_effect->Reload( (const EFK_CHAR*)L"effect_reload1.efk" );
#else
		g_client->Reload( g_manager, (const EFK_CHAR*)L"effect_reload1.efk", (const EFK_CHAR*)L"test" );		
#endif
	}

	
#ifndef __DIRECT
	g_server->Update();
#endif

	g_manager->Flip();

	g_manager->Update( 1.0f );

	g_count++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif