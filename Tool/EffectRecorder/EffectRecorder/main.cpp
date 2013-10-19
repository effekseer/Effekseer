
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#include <XAudio2.h>
#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3dx9.lib" )

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Debug.lib" )
#pragma comment(lib, "EffekseerSoundXAudio2.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Release.lib" )
#pragma comment(lib, "EffekseerSoundXAudio2.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct EffectData
{
	std::wstring Path;
	Effekseer::Vector3D Position;
	Effekseer::Vector3D Focus;
	int32_t				Time;

	EffectData( const wchar_t* path, int32_t time, Effekseer::Vector3D position, Effekseer::Vector3D focus )
		: Path	( path )
		, Time	( time )
		, Position	( position )
		, Focus		( focus )
	{
		
	}
};

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

static LPDIRECT3D9						g_d3d = NULL;
static LPDIRECT3DDEVICE9				g_d3d_device = NULL;
static IXAudio2*						g_xa2 = NULL;
static IXAudio2MasteringVoice*			g_xa2_master = NULL;

static std::vector<EffectData>			g_data;

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
	wchar_t szClassNme[]      =  L"EffectRecorder";
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
		L"EffectRecorder",
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

	// DirectX9の初期化を行う
	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = g_window_width;
	d3dp.BackBufferHeight = g_window_height;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dp.BackBufferCount = 1;      
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = g_window_handle;
	d3dp.EnableAutoDepthStencil = TRUE;
    d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	
	g_d3d->CreateDevice( 
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		g_window_handle,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dp,
		&g_d3d_device );
	
	// XAudio2の初期化を行う
	XAudio2Create( &g_xa2 );

	g_xa2->CreateMasteringVoice( &g_xa2_master );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void MainLoop()
{
	Sleep( 10000 );

	int32_t time = 0;
	EffectData* data = nullptr;
	int32_t index = 0;

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
			if( time == 30 )
			{
				g_manager->StopEffect( g_handle );
				ES_SAFE_RELEASE( g_effect );

				if( index < g_data.size() )
				{
					data = &(g_data[ index ]);

					// カメラ行列を設定
					g_renderer->SetCameraMatrix(
						::Effekseer::Matrix44().LookAtRH( data->Position, data->Focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
					
					// エフェクトの読込
					g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)data->Path.c_str() );

					// エフェクトの再生
					g_handle = g_manager->Play( g_effect, 0, 0, 0 );
				}
				else
				{
					data = nullptr;
				}

				index++;
			}

			// エフェクトの更新処理を行う
			g_manager->Update();
			
			
			g_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
			g_d3d_device->BeginScene();

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// エフェクトの描画を行う。
			g_manager->Draw();

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			g_d3d_device->EndScene();

			time++;

			if( data != nullptr && time == data->Time + 30 )
			{
				time = 0;
			
				// エフェクトの停止
				g_manager->StopRoot( g_handle );
			}

			{
				HRESULT hr;
				hr = g_d3d_device->Present( NULL, NULL, NULL, NULL );

				// デバイスロスト処理
				switch ( hr )
				{
					// デバイスロスト
					case D3DERR_DEVICELOST:
					while ( FAILED( hr = g_d3d_device->TestCooperativeLevel() ) )
					{
						switch ( hr )
						{
							// デバイスロスト
							case D3DERR_DEVICELOST:
								::SleepEx( 1000, true );
								break;

							// デバイスロスト：リセット可
							case D3DERR_DEVICENOTRESET:
								
								// デバイスロストの処理を行う前に実行する
								g_renderer->OnLostDevice();

								D3DPRESENT_PARAMETERS d3dp;
								ZeroMemory(&d3dp, sizeof(d3dp));
								d3dp.BackBufferWidth = g_window_width;
								d3dp.BackBufferHeight = g_window_height;
								d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
								d3dp.BackBufferCount = 1;      
								d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
								d3dp.Windowed = TRUE;
								d3dp.hDeviceWindow = g_window_handle;
								d3dp.EnableAutoDepthStencil = TRUE;
								d3dp.AutoDepthStencilFormat = D3DFMT_D16;

								g_d3d_device->Reset( &d3dp );

								// デバイスロストの処理の後に実行する
								g_renderer->OnResetDevice();

								break;
						}
					}
					break;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	// 再生設定
	g_data.push_back( EffectData( L"AddEnemy.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Aya_Attack.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"BlazingStar.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Boss_Death.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"BreakUnit.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Charge.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"CorrectItem1.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"CorrectItem2.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"CorrectItem3.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Explosion.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ) * 0.5f, Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Flan_Fire.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ) * 0.5f, Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"GetKey.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"GetKeyItem1.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"GetKeyItem2.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"GetLife.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Marisa_Damage.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Marisa_Laser.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Marisa_MasterSpark.efk", 200, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Marisa_Shot.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"OpenKey.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	g_data.push_back( EffectData( L"Tenshi_Sword.efk", 100, ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ), Effekseer::Vector3D() ) );
	
	InitWindow();
	
	// 描画用インスタンスの生成
	g_renderer = ::EffekseerRenderer::Renderer::Create( g_d3d_device, 2000 );
	
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
	g_sound = ::EffekseerSound::Sound::Create( g_xa2, 16, 16 );

	// 音再生用インスタンスから再生機能を指定
	g_manager->SetSoundPlayer( g_sound->CreateSoundPlayer() );
	
	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetSoundLoader( g_sound->CreateSoundLoader() );

	// 視点位置を確定
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );

	MainLoop();

	// エフェクトの破棄
	ES_SAFE_RELEASE( g_effect );

	// 先にエフェクト管理用インスタンスを破棄
	g_manager->Destroy();

	// 次に音再生用インスタンスを破棄
	g_sound->Destory();

	// 次に描画用インスタンスを破棄
	g_renderer->Destory();

	// XAudio2の解放
	if( g_xa2_master != NULL )
	{
		g_xa2_master->DestroyVoice();
		g_xa2_master = NULL;
	}
	ES_SAFE_RELEASE( g_xa2 );

	// DirectXの解放
	ES_SAFE_RELEASE( g_d3d_device );
	ES_SAFE_RELEASE( g_d3d );

	// COMの終了処理
	CoUninitialize();

	return 0;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------