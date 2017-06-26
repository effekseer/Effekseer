
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <assert.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <AL/alc.h>
#include <Wingdi.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "winmm.lib")

#pragma comment(lib, "OpenAL32.lib" )

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <EffekseerSoundAL.h>

#if _DEBUG
#pragma comment(lib, "VS2013/Debug/Effekseer.lib" )
#pragma comment(lib, "VS2013/Debug/EffekseerRendererGL.lib" )
#pragma comment(lib, "VS2013/Debug/EffekseerSoundAL.lib" )
#else
#pragma comment(lib, "VS2013/Release/Effekseer.lib" )
#pragma comment(lib, "VS2013/Release/EffekseerRendererGL.lib" )
#pragma comment(lib, "VS2013/Release/EffekseerSoundAL.lib" )
#endif

typedef int (APIENTRY * PFNWGLSWAPINTERVALEXTPROC)(int);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;
static ::EffekseerSound::Sound*			g_sound = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;

static HDC								g_hDC = NULL;
static HGLRC							g_hGLRC = NULL;

static ALCdevice*						g_alcdev = NULL;
static ALCcontext*						g_alcctx = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static void WaitFrame()
{
	static DWORD beforeTime = timeGetTime();
	DWORD currentTime = timeGetTime();
	
	DWORD elapsedTime = currentTime - beforeTime;
	if (elapsedTime < 16) {
		Sleep(16 - elapsedTime);
	}
	beforeTime = timeGetTime();
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
	wchar_t szClassNme[]      =  L"RuntimeSample";
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
		L"RuntimeSample",
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
	
	// COMの初期化
	CoInitializeEx( NULL, NULL );

	// OpenGLの初期化を行う
	g_hDC = GetDC( g_window_handle );

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA, 24,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32,
		0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pxfm = ChoosePixelFormat( g_hDC, &pfd );

	SetPixelFormat( g_hDC, pxfm, &pfd );

	g_hGLRC = wglCreateContext( g_hDC );

	wglMakeCurrent( g_hDC, g_hGLRC );

	auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);

	glViewport( 0, 0, g_window_width, g_window_height );
	
	// OpenALデバイスを作成
	g_alcdev = alcOpenDevice(NULL);

	// OpenALコンテキストを作成
	g_alcctx = alcCreateContext(g_alcdev, NULL);

	alcMakeContextCurrent(g_alcctx);
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
			// エフェクトの移動処理を行う
			g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );

			// エフェクトの更新処理を行う
			g_manager->Update();
			
			
			wglMakeCurrent( g_hDC, g_hGLRC );

			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// エフェクトの描画を行う。
			g_manager->Draw();

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			glFlush();
			wglMakeCurrent( 0, 0 );
			WaitFrame();
			SwapBuffers( g_hDC );
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	InitWindow();
	
	// 描画用インスタンスの生成
	g_renderer = ::EffekseerRendererGL::Renderer::Create( 2000 );
	
	// エフェクト管理用インスタンスの生成
	g_manager = ::Effekseer::Manager::Create( 2000 );

	// 描画用インスタンスから描画機能を設定
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );

	// 音再生用インスタンスの生成
	g_sound = EffekseerSound::Sound::Create( 32 );

	// 音再生用インスタンスから再生機能を指定
	g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	
	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );

	// 視点位置を確定
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH_OpenGL( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );

	// カメラ行列を設定
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH( g_position, ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ), ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	
	// エフェクトの読込
	g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"test.efk" );

	// エフェクトの再生
	g_handle = g_manager->Play( g_effect, 0, 0, 0 );

	MainLoop();
	
	// エフェクトの停止
	g_manager->StopEffect( g_handle );

	// エフェクトの破棄
	ES_SAFE_RELEASE( g_effect );

	// 先にエフェクト管理用インスタンスを破棄
	g_manager->Destroy();

	// 次に音再生用インスタンスを破棄
	g_sound->Destroy();

	// 次に描画用インスタンスを破棄
	g_renderer->Destroy();

	// OpenALの解放
	alcDestroyContext(g_alcctx);
	alcCloseDevice(g_alcdev);
	
	g_alcctx = NULL;
	g_alcdev = NULL;

	// OpenGLの解放
	wglMakeCurrent( 0, 0 );
	wglDeleteContext( g_hGLRC );
	timeEndPeriod(1);

	// COMの終了処理
	CoUninitialize();

	return 0;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------