
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <string>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <d3d9.h>
#include <XAudio2.h>
#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "xaudio2.lib" )

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static HWND g_window_handle = NULL;
static int g_window_width = 800;
static int g_window_height = 600;
static ::Effekseer::ManagerRef				g_manager;
static ::EffekseerRendererDX9::RendererRef	g_renderer;
static ::EffekseerSound::SoundRef			g_sound = NULL;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::EffectRef			g_effect;
static ::Effekseer::Handle				g_handle = -1;

static LPDIRECT3D9						g_d3d = NULL;
static LPDIRECT3DDEVICE9				g_d3d_device = NULL;
static IXAudio2*						g_xa2 = NULL;
static IXAudio2MasteringVoice*			g_xa2_master = NULL;

static int32_t							g_timer = 0;

static IDirect3DSurface9*				g_backgroundSurface = nullptr;
static IDirect3DTexture9*				g_backgroundTexture = nullptr;

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
static bool CopyRenderTargetToBackground()
{
	bool ret = false;

	HRESULT hr;
	IDirect3DSurface9* tempRender = nullptr;
	IDirect3DSurface9* tempDepth = nullptr;

	hr = g_d3d_device->GetRenderTarget(0, &tempRender);
	if (FAILED(hr))
	{
		goto Exit;
	}

	hr = g_d3d_device->GetDepthStencilSurface(&tempDepth);
	if (FAILED(hr))
	{
		goto Exit;
	}

	g_d3d_device->SetRenderTarget(0, g_backgroundSurface);
	g_d3d_device->SetDepthStencilSurface(nullptr);

	D3DSURFACE_DESC desc;
	tempRender->GetDesc(&desc);
	
	g_d3d_device->StretchRect(
		tempRender,
		nullptr,
		g_backgroundSurface,
		nullptr,
		D3DTEXF_POINT
		);

	g_d3d_device->SetRenderTarget(0, tempRender);
	g_d3d_device->SetDepthStencilSurface(tempDepth);
	
	ret = true;

Exit:;
	ES_SAFE_RELEASE(tempRender);
	ES_SAFE_RELEASE(tempDepth);

	return ret;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class DistortingCallback
	: public EffekseerRenderer::DistortingCallback
{
public:
	DistortingCallback()
	{

	}

	virtual ~DistortingCallback()
	{

	}

	bool OnDistorting()
	{
		if (g_backgroundTexture == NULL)
		{
			g_renderer->SetBackground(NULL);
			return false;
		}

		CopyRenderTargetToBackground();
		g_renderer->SetBackground(g_backgroundTexture);
		return true;
	}
};


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
			// エフェクトの更新処理を行う。
			g_manager->Update();
		
			g_d3d_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
			g_d3d_device->BeginScene();

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// Render rear effects
			// 背面のエフェクトの描画を行う。
			g_manager->DrawBack();

			// Distort background and rear effects.
			// 背景と背面のエフェクトを歪ませる。
			DistortingCallback distoring;
			distoring.OnDistorting();

			// Render front effects
			// 前面のエフェクトの描画を行う。
			g_manager->DrawFront();

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			g_d3d_device->EndScene();

			g_timer++;

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

#if _WIN32
#include <windows.h>
std::wstring ToWide(const char* pText);
void GetDirectoryName(char* dst, char* src);
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main(int argc, char **argv)
{
#if _WIN32
	char current_path[MAX_PATH + 1];
	GetDirectoryName(current_path, argv[0]);
	SetCurrentDirectoryA(current_path);
#endif

	InitWindow();

	// 描画用インスタンスの生成
	g_renderer = ::EffekseerRendererDX9::Renderer::Create( g_d3d_device, 2000 );
	
	// Specify a distortion function
	// 歪み機能を設定

	// If you'd like to distort background and particles by rendering, it need to specify it.
	// It is a bit heavy
	// もし、描画するごとに背景とパーティクルを歪ませたい場合、設定する必要がある
	// やや重い
	g_renderer->SetDistortingCallback(new DistortingCallback());

	// 背景バッファの生成
	g_d3d_device->CreateTexture(
		640,
		480,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&g_backgroundTexture,
		NULL
		);

	g_backgroundTexture->GetSurfaceLevel(0, &g_backgroundSurface);

	// エフェクト管理用インスタンスの生成
	g_manager = ::Effekseer::Manager::Create( 2000 );

	// 描画用インスタンスから描画機能を設定
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );
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
	g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f) * 0.25;

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f));

	// カメラ行列を設定
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// エフェクトの読込
	g_effect = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_ASSETS_DIR_U16 u"distortion.efk");

	// エフェクトの再生
	g_handle = g_manager->Play(g_effect, 0, 0, 0);
	
	MainLoop();

	// エフェクトの停止
	g_manager->StopEffect(g_handle);

	// 先にエフェクト管理用インスタンスを破棄
	g_manager.Reset();

	// 次に音再生用インスタンスを破棄
	g_sound.Reset();

	// 次に描画用インスタンスを破棄
	g_renderer.Reset();

	// XAudio2の解放
	if( g_xa2_master != NULL )
	{
		g_xa2_master->DestroyVoice();
		g_xa2_master = NULL;
	}
	ES_SAFE_RELEASE( g_xa2 );

	// バックバッファの破棄
	ES_SAFE_RELEASE( g_backgroundTexture );
	ES_SAFE_RELEASE( g_backgroundSurface );

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

#if _WIN32
static std::wstring ToWide(const char* pText)
{
	int Len = ::MultiByteToWideChar(CP_ACP, 0, pText, -1, NULL, 0);

	wchar_t* pOut = new wchar_t[Len + 1];
	::MultiByteToWideChar(CP_ACP, 0, pText, -1, pOut, Len);
	std::wstring Out(pOut);
	delete[] pOut;

	return Out;
}

void GetDirectoryName(char* dst, char* src)
{
	auto Src = std::string(src);
	int pos = 0;
	int last = 0;
	while (Src.c_str()[pos] != 0)
	{
		dst[pos] = Src.c_str()[pos];

		if (Src.c_str()[pos] == L'\\' || Src.c_str()[pos] == L'/')
		{
			last = pos;
		}

		pos++;
	}

	dst[pos] = 0;
	dst[last] = 0;
}
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------