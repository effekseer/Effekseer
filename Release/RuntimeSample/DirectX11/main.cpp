
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <d3d11.h>
#include <XAudio2.h>
#pragma comment(lib, "d3d11.lib" )

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>

#if _DEBUG
#pragma comment(lib, "VS2013/Debug/Effekseer.lib" )
#pragma comment(lib, "VS2013/Debug/EffekseerRendererDX11.lib" )
#pragma comment(lib, "VS2013/Debug/EffekseerSoundXAudio2.lib" )
#else
#pragma comment(lib, "VS2013/Release/Effekseer.lib" )
#pragma comment(lib, "VS2013/Release/EffekseerRendererDX11.lib" )
#pragma comment(lib, "VS2013/Release/EffekseerSoundXAudio2.lib" )
#endif

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

static ID3D11Device*				g_device = NULL;
static ID3D11DeviceContext*			g_context = NULL;
static IDXGIDevice1*				g_dxgiDevice = NULL;
static IDXGIAdapter*				g_adapter = NULL;
static IDXGIFactory*				g_dxgiFactory = NULL;
static IDXGISwapChain*				g_swapChain = NULL;
static ID3D11Texture2D*				g_backBuffer = NULL;
static ID3D11Texture2D*				g_depthBuffer = NULL;
static ID3D11RenderTargetView*		g_renderTargetView = NULL;
static ID3D11DepthStencilView*		g_depthStencilView = NULL;

static IXAudio2*						g_xa2 = NULL;
static IXAudio2MasteringVoice*			g_xa2_master = NULL;

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

	// DirectX11の初期化を行う
	UINT debugFlag = 0;
	//debugFlag = D3D11_CREATE_DEVICE_DEBUG;

	auto hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		debugFlag,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&g_device,
		NULL,
		&g_context );

	if FAILED( hr )
	{
		goto End;
	}

	if( FAILED(g_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&g_dxgiDevice)) )
	{
		goto End;
	}

	if( FAILED(g_dxgiDevice->GetAdapter(&g_adapter)) )
	{
		goto End;
	}

	g_adapter->GetParent( __uuidof(IDXGIFactory), (void**)&g_dxgiFactory );
	if( g_dxgiFactory == NULL )
	{
		goto End;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = g_window_width;
	hDXGISwapChainDesc.BufferDesc.Height = g_window_height;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator  = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)g_window_handle;
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if( FAILED(g_dxgiFactory->CreateSwapChain(g_device, &hDXGISwapChainDesc, &g_swapChain)) )
	{
		goto End;
	}

	if( FAILED( g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_backBuffer)) )
	{
		goto End;
	}

	if( FAILED(g_device->CreateRenderTargetView(g_backBuffer, NULL, &g_renderTargetView)) )
	{
		goto End;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width			= hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height			= hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels		= 1;
	hTexture2dDesc.ArraySize		= 1;
	hTexture2dDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc		= hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage			= D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    hTexture2dDesc.CPUAccessFlags	= 0;
    hTexture2dDesc.MiscFlags		= 0;
	if(FAILED(g_device->CreateTexture2D(&hTexture2dDesc, NULL, &g_depthBuffer)))
	{
		goto End;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format		= hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags			= 0;
	if(FAILED(g_device->CreateDepthStencilView(g_depthBuffer, &hDepthStencilViewDesc, &g_depthStencilView)))
	{
		goto End;
	}

	g_context->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)g_window_width;
	vp.Height = (float)g_window_height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_context->RSSetViewports(1, &vp);
	
	// XAudio2の初期化を行う
	XAudio2Create( &g_xa2 );

	g_xa2->CreateMasteringVoice( &g_xa2_master );

	return;
End:
	ES_SAFE_RELEASE( g_renderTargetView );
	ES_SAFE_RELEASE( g_backBuffer );
	ES_SAFE_RELEASE( g_depthStencilView );
	ES_SAFE_RELEASE( g_depthBuffer );
	ES_SAFE_RELEASE( g_swapChain );
	ES_SAFE_RELEASE( g_dxgiFactory );
	ES_SAFE_RELEASE( g_adapter );
	ES_SAFE_RELEASE( g_dxgiDevice );
	ES_SAFE_RELEASE( g_context );
	ES_SAFE_RELEASE( g_device );
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
			
			float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
			g_context->ClearRenderTargetView( g_renderTargetView, ClearColor);
			g_context->ClearDepthStencilView( g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// エフェクトの描画を行う。
			g_manager->Draw();

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			
			g_swapChain->Present(1, 0);
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
	g_renderer = ::EffekseerRendererDX11::Renderer::Create( g_device, g_context, 2000 );
	
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
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f );

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );

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

	// XAudio2の解放
	if( g_xa2_master != NULL )
	{
		g_xa2_master->DestroyVoice();
		g_xa2_master = NULL;
	}
	ES_SAFE_RELEASE( g_xa2 );

	// DirectXの解放
	ES_SAFE_RELEASE( g_renderTargetView );
	ES_SAFE_RELEASE( g_backBuffer );
	ES_SAFE_RELEASE( g_depthStencilView );
	ES_SAFE_RELEASE( g_depthBuffer );
	ES_SAFE_RELEASE( g_swapChain );
	ES_SAFE_RELEASE( g_dxgiFactory );
	ES_SAFE_RELEASE( g_adapter );
	ES_SAFE_RELEASE( g_dxgiDevice );
	ES_SAFE_RELEASE( g_context );
	ES_SAFE_RELEASE( g_device );

	// COMの終了処理
	CoUninitialize();

	return 0;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------