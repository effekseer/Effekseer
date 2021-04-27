
#include <stdio.h>
#include <string>
#include <windows.h>

#include <XAudio2.h>
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "xaudio2.lib")

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>
#include <Window.h>

std::shared_ptr<Utils::Window> g_window;
bool InitializeWindowAndDevice();
void TerminateWindowAndDevice();
void ClearScreen();
void PresentDevice();

ID3D11Device* GetID3D11Device();
ID3D11DeviceContext* GetID3D11DeviceContext();
IXAudio2* GetIXAudio2();

int main(int argc, char** argv)
{
	InitializeWindowAndDevice();

		// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// However, if it is disposed before the end of COM, it will go wrong, so we add a scope.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。
	// ただし、COMの終了前に削除されるとおかしくなるので、スコープを追加する。
	{
		// Create a renderer of effects
		// エフェクトのレンダラーの作成
		ID3D11Device* d3D11Device = GetID3D11Device();
		ID3D11DeviceContext* d3D11DeviceContext = GetID3D11DeviceContext();
		auto renderer = ::EffekseerRendererDX11::Renderer::Create(d3D11Device, d3D11DeviceContext, 8000);

		// Create a manager of effects
		// エフェクトのマネージャーの作成
		auto manager = ::Effekseer::Manager::Create(8000);

		// Sprcify rendering modules
		// 描画モジュールの設定
		manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
		manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
		manager->SetRingRenderer(renderer->CreateRingRenderer());
		manager->SetTrackRenderer(renderer->CreateTrackRenderer());
		manager->SetModelRenderer(renderer->CreateModelRenderer());

		// Specify a texture, model, curve and material loader
		// It can be extended by yourself. It is loaded from a file on now.
		// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
		// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
		manager->SetTextureLoader(renderer->CreateTextureLoader());
		manager->SetModelLoader(renderer->CreateModelLoader());
		manager->SetMaterialLoader(renderer->CreateMaterialLoader());
		manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

		// Specify sound modules
		// サウンドモジュールの設定
		IXAudio2* xAudio2 = GetIXAudio2();
		auto sound = ::EffekseerSound::Sound::Create(xAudio2, 16, 16);

		// Specify a metho to play sound from an instance of sound
		// 音再生用インスタンスから再生機能を指定
		manager->SetSoundPlayer(sound->CreateSoundPlayer());

		// Specify a sound data loader
		// It can be extended by yourself. It is loaded from a file on now.
		// サウンドデータの読込機能を設定する。
		// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
		manager->SetSoundLoader(sound->CreateSoundLoader());

		// Specify a position of view
		// 視点位置を確定
		auto g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

		// Specify a projection matrix
		// 投影行列を設定
		renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH(
			90.0f / 180.0f * 3.14f, (float)g_window->GetWindowSize().X / (float)g_window->GetWindowSize().Y, 1.0f, 500.0f));

		// Specify a camera matrix
		// カメラ行列を設定
		renderer->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		// Load an effect
		// エフェクトの読込
		auto effect = Effekseer::Effect::Create(manager, EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efk");

		int32_t time = 0;
		Effekseer::Handle handle = 0;

		while (g_window->OnNewFrame())
		{
			if (time % 120 == 0)
			{
				// Play an effect
				// エフェクトの再生
				handle = manager->Play(effect, 0, 0, 0);
			}

			if (time % 120 == 119)
			{
				// Stop effects
				// エフェクトの停止
				manager->StopEffect(handle);
			}

			// Move the effect
			// エフェクトの移動
			manager->AddLocation(handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

			// Update the manager
			// マネージャーの更新
			manager->Update();

			// Ececute functions about DirectX
			// DirectXの処理
			ClearScreen();

			// Update a time
			// 時間を更新する
			renderer->SetTime(time / 60.0f);

			// Begin to rendering effects
			// エフェクトの描画開始処理を行う。
			renderer->BeginRendering();

			// Render effects
			// エフェクトの描画を行う。
			manager->Draw();

			// Finish to rendering effects
			// エフェクトの描画終了処理を行う。
			renderer->EndRendering();

			// Ececute functions about DirectX
			// DirectXの処理
			PresentDevice();

			time++;
		}

		// Dispose the manager
		// マネージャーの破棄
		manager.Reset();

		// Dispose the sound
		// サウンドの破棄
		sound.Reset();

		// Dispose the renderer
		// レンダラーの破棄
		renderer.Reset();
	}

	TerminateWindowAndDevice();

	return 0;
}

static IDXGIDevice1* g_dxgiDevice = nullptr;
static IDXGIAdapter* g_adapter = nullptr;
static IDXGIFactory* g_dxgiFactory = nullptr;
static IDXGISwapChain* g_swapChain = nullptr;
static ID3D11Texture2D* g_backBuffer = nullptr;
static ID3D11Texture2D* g_depthBuffer = nullptr;
static ID3D11RenderTargetView* g_renderTargetView = nullptr;
static ID3D11DepthStencilView* g_depthStencilView = nullptr;

static ID3D11Device* g_device = nullptr;
static ID3D11DeviceContext* g_context = nullptr;

static IXAudio2* g_xa2 = nullptr;
static IXAudio2MasteringVoice* g_xa2_master = nullptr;

ID3D11Device* GetID3D11Device() { return g_device; }
ID3D11DeviceContext* GetID3D11DeviceContext() { return g_context; }
IXAudio2* GetIXAudio2() { return g_xa2; }

bool InitializeWindowAndDevice()
{
	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// Initialize Window
	// ウインドウの初期化
	g_window = Utils::Window::Create("DirectX11", Utils::Vec2I{1280, 720});

	// Initialize DirectX11
	// DirectX11の初期化
	UINT debugFlag = 0;
	// debugFlag = D3D11_CREATE_DEVICE_DEBUG;

	auto hr = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, debugFlag, nullptr, 0, D3D11_SDK_VERSION, &g_device, nullptr, &g_context);

	if
		FAILED(hr) { goto End; }

	if (FAILED(g_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&g_dxgiDevice)))
	{
		goto End;
	}

	if (FAILED(g_dxgiDevice->GetAdapter(&g_adapter)))
	{
		goto End;
	}

	g_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&g_dxgiFactory);
	if (g_dxgiFactory == nullptr)
	{
		goto End;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = g_window->GetWindowSize().X;
	hDXGISwapChainDesc.BufferDesc.Height = g_window->GetWindowSize().Y;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)g_window->GetNativePtr(0);
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(g_dxgiFactory->CreateSwapChain(g_device, &hDXGISwapChainDesc, &g_swapChain)))
	{
		goto End;
	}

	if (FAILED(g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_backBuffer)))
	{
		goto End;
	}

	if (FAILED(g_device->CreateRenderTargetView(g_backBuffer, nullptr, &g_renderTargetView)))
	{
		goto End;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	if (FAILED(g_device->CreateTexture2D(&hTexture2dDesc, nullptr, &g_depthBuffer)))
	{
		goto End;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(g_device->CreateDepthStencilView(g_depthBuffer, &hDepthStencilViewDesc, &g_depthStencilView)))
	{
		goto End;
	}

	g_context->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)g_window->GetWindowSize().X;
	vp.Height = (float)g_window->GetWindowSize().Y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_context->RSSetViewports(1, &vp);

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(&g_xa2);

	g_xa2->CreateMasteringVoice(&g_xa2_master);

	return true;
End:
	ES_SAFE_RELEASE(g_renderTargetView);
	ES_SAFE_RELEASE(g_backBuffer);
	ES_SAFE_RELEASE(g_depthStencilView);
	ES_SAFE_RELEASE(g_depthBuffer);
	ES_SAFE_RELEASE(g_swapChain);
	ES_SAFE_RELEASE(g_dxgiFactory);
	ES_SAFE_RELEASE(g_adapter);
	ES_SAFE_RELEASE(g_dxgiDevice);
	ES_SAFE_RELEASE(g_context);
	ES_SAFE_RELEASE(g_device);

	return false;
}

void TerminateWindowAndDevice()
{
	// Release XAudio2
	// XAudio2の解放
	if (g_xa2_master != nullptr)
	{
		g_xa2_master->DestroyVoice();
		g_xa2_master = nullptr;
	}
	ES_SAFE_RELEASE(g_xa2);

	// Release DirectX
	// DirectXの解放
	ES_SAFE_RELEASE(g_renderTargetView);
	ES_SAFE_RELEASE(g_backBuffer);
	ES_SAFE_RELEASE(g_depthStencilView);
	ES_SAFE_RELEASE(g_depthBuffer);
	ES_SAFE_RELEASE(g_swapChain);
	ES_SAFE_RELEASE(g_dxgiFactory);
	ES_SAFE_RELEASE(g_adapter);
	ES_SAFE_RELEASE(g_dxgiDevice);
	ES_SAFE_RELEASE(g_context);
	ES_SAFE_RELEASE(g_device);

	g_window = nullptr;

	// Release COM
	// COMの解放
	CoUninitialize();
}

void ClearScreen()
{
	float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	g_context->ClearRenderTargetView(g_renderTargetView, ClearColor);
	g_context->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void PresentDevice() { g_swapChain->Present(1, 0); }