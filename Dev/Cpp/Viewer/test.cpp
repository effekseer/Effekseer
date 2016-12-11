
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.h"
#include "EffekseerRenderer/EffekseerRendererDX9.Renderer.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerTool::Renderer*		g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EFK_STDCALL EffectInstanceRemovingCallback ( ::Effekseer::Manager* manager, ::Effekseer::Handle handle, bool isRemovingManager )
{
	printf("remove effect.\n");
}

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
		L"Effekseer",
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
			g_manager->Update();
				
			g_renderer->BeginRendering();
			g_manager->Draw();
			g_renderer->EndRendering();
			g_renderer->Present();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	InitWindow();
	
	g_renderer = new ::EffekseerTool::Renderer( 2000, false );
	g_renderer->Initialize( g_window_handle, g_window_width, g_window_height );
	
	g_manager = ::Effekseer::Manager::Create( 2000 );
	g_manager->SetSpriteRenderer( g_renderer->GetRenderer()->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->GetRenderer()->CreateRibbonRenderer() );
	g_manager->SetTextureLoader( g_renderer->GetRenderer()->CreateTextureLoader() );
	
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

	g_renderer->GetRenderer()->SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( g_position, ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ), ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	g_renderer->RendersGuide = true;
	g_renderer->GuideWidth = 100;
	g_renderer->GuideHeight = 100;

	char* pData = NULL;
	FILE* fp = _wfopen( L"Resource/test.efk", L"rb" );
	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	pData = new char[size];
	fseek( fp, 0, SEEK_SET );
	fread( pData, 1, size, fp );
	fclose( fp );

	g_effect = Effekseer::Effect::Create( g_manager, (void*)pData, size );

	delete [] pData;

	// 録画テスト
	/*
	{
		g_handle = g_manager->Play( g_effect, 0, 0, 0 );

		g_manager->Update();

		g_renderer->BeginRecord( g_renderer->GuideWidth * 8, g_renderer->GuideHeight );

		for( int i = 0; i < 8; i++ )
		{
			g_renderer->SetRecordRect( g_renderer->GuideWidth * i, 0 );

			g_renderer->BeginRendering();
			g_manager->Draw( g_position );
			g_renderer->EndRendering();

			for( int j = 0; j < 8; j++ )
			{
				g_manager->Update();
			}
		}

		g_renderer->EndRecord( L"tmp.png" );

		g_manager->StopEffect( g_handle );
		g_handle = -1;
	}
	*/

	g_handle = g_manager->Play( g_effect, 0, 0, 0 );

	g_manager->SetRemovingCallback( g_handle, EffectInstanceRemovingCallback );
	MainLoop();
	
	g_manager->StopEffect( g_handle );

	ES_SAFE_RELEASE( g_effect );

	g_manager->Destroy();
	ES_SAFE_DELETE( g_renderer );

	return 0;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------