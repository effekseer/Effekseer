#if 1

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <assert.h>
#include <windows.h>
#include <vector>

#include "graphics.h"
#include "sound.h"

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#define __DicrectX9	1
#define __OpenGL	0

#define __NormalMode 1
#define __PerformanceCheckMode 0

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop();
void Init();
void PlayEffect();

static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;

::Effekseer::Manager*					g_manager = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::Vector3D			g_focus;

static std::vector < ::Effekseer::Effect *>	g_effects;

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
		case WM_LBUTTONDOWN:
			PlayEffect();
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
	WNDCLASSW wndClass;
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
	RegisterClassW(&wndClass);
	g_window_handle = CreateWindowW(
		szClassNme,
		L"Effekseer Test Program (ÉNÉäÉbÉNÇ∑ÇÈÇ∆çƒê∂)",
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

			static int count = 0;

			//if (count < 50)
			{
				g_manager->Update();
			}

			count++;

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

	g_manager = ::Effekseer::Manager::Create( 2000 );

	InitGraphics( g_window_handle, g_window_width, g_window_height );

	InitSound( g_window_handle );

	Init();

	MainLoop();

	g_manager->Destroy();

	for (size_t i = 0; i < g_effects.size(); i++)
	{
		ES_SAFE_RELEASE(g_effects[i]);
	}

	TermSound();

	TermGraphics();

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

	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Laser01.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Laser02.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Ribbon_Parent.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Ribbon_Sword.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Ring_Shape1.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Ring_Shape2.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Sprite_FixedYAxis.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"Simple_Track1.efk" ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"block.efk" ) );
	
	PlayEffect();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop()
{
#if __NormalMode
	//g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );
#endif

#if __PerformanceCheckMode
	static int count = 0;

	if( count % 10 == 1 && count < 100 )
	{
		auto updateTime = g_manager->GetUpdateTime();
		auto drawTime = g_manager->GetDrawTime();

		printf("UpdateTime %d, DrawTime %d\n", updateTime, drawTime );
	}

	count++;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PlayEffect()
{
#if __NormalMode
	static int target = 0;
	target = target % g_effects.size();
	g_handle = g_manager->Play( g_effects[target], 0, 0, 0 );
	target++;
	//g_manager->SetLocation( g_handle, -5.0f, 0.0f, -20.0f );

	::Effekseer::Matrix43 baseMat;
	baseMat.Scaling( 1.0f, 1.0f, 1.0f );
	g_manager->SetBaseMatrix( g_handle, baseMat );
#endif

#if __PerformanceCheckMode
	
	for( float y = -10.0f; y <= 10.0f; y += 2.0f )
	{
		for( float x = -10.0f; x <= 10.0f; x += 2.0f )
		{
			auto handle = g_manager->Play( g_effect, x, y, 0 );
			//g_manager->SetShown( handle, false );
		}
	}
	//g_manager->Play( g_effect, 0, 0, 0 );
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif