
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <vector>
#include <windows.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <XAudio2.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "xaudio2.lib")

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
static ::Effekseer::ManagerRef g_manager;
static ::EffekseerRenderer::RendererRef g_renderer = NULL;
static ::EffekseerSound::SoundRef g_sound = NULL;
static ::Effekseer::EffectRef g_effects[3];
static ::Effekseer::Vector3D g_position;

static LPDIRECT3D9 g_d3d = NULL;
static LPDIRECT3DDEVICE9 g_d3d_device = NULL;
static IXAudio2* g_xa2 = NULL;
static IXAudio2MasteringVoice* g_xa2_master = NULL;

static std::vector<::Effekseer::Handle> g_handles;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
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
	wchar_t szClassNme[] = L"SortHandle";
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(0);
	wndClass.hIcon = NULL;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szClassNme;
	RegisterClass(&wndClass);
	g_window_handle = CreateWindow(szClassNme,
								   L"SortHandle",
								   WS_SYSMENU,
								   CW_USEDEFAULT,
								   CW_USEDEFAULT,
								   g_window_width,
								   g_window_height,
								   NULL,
								   NULL,
								   GetModuleHandle(0),
								   NULL);
	ShowWindow(g_window_handle, true);
	UpdateWindow(g_window_handle);

	// COMの初期化
	CoInitializeEx(NULL, NULL);

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

	g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_window_handle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &g_d3d_device);

	// XAudio2の初期化を行う
	XAudio2Create(&g_xa2);

	g_xa2->CreateMasteringVoice(&g_xa2_master);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void MainLoop()
{
	int time = 0;
	bool reverse = false;
	for (;;)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return;
			}
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (time % 120 == 0)
			{
				// エフェクトの停止
				for (size_t i = 0; i < g_handles.size(); i++)
				{
					g_manager->StopEffect(g_handles[i]);
				}
				g_handles.clear();

				// エフェクトの再生
				for (int i = 0; i < 3; i++)
				{
					Effekseer::Handle handle = g_manager->Play(g_effects[i], (i - 1) * 10.0f, 0, 0);
					g_handles.push_back(handle);
				}

				reverse = !reverse;
			}

			// エフェクトの更新処理を行う
			g_manager->Update();

			g_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			g_d3d_device->BeginScene();

			// エフェクトの描画開始処理を行う。
			g_renderer->BeginRendering();

			// エフェクトを個別に描画を行う。
			if (reverse)
			{
				for (int i = 0; i < g_handles.size(); i++)
				{
					g_manager->DrawHandle(g_handles[i]);
				}
			}
			else
			{
				for (int i = g_handles.size() - 1; i > -1; i--)
				{
					g_manager->DrawHandle(g_handles[i]);
				}
			}

			// エフェクトの描画終了処理を行う。
			g_renderer->EndRendering();

			g_d3d_device->EndScene();

			time++;

			{
				HRESULT hr;
				hr = g_d3d_device->Present(NULL, NULL, NULL, NULL);

				// デバイスロスト処理
				switch (hr)
				{
				// デバイスロスト
				case D3DERR_DEVICELOST:
					while (FAILED(hr = g_d3d_device->TestCooperativeLevel()))
					{
						switch (hr)
						{
						// デバイスロスト
						case D3DERR_DEVICELOST:
							::SleepEx(1000, true);
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

							g_d3d_device->Reset(&d3dp);

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
int main(int argc, char** argv)
{
#if _WIN32
	char current_path[MAX_PATH + 1];
	GetDirectoryName(current_path, argv[0]);
	SetCurrentDirectoryA(current_path);
#endif

	InitWindow();

	// 描画用インスタンスの生成
	g_renderer = ::EffekseerRendererDX9::Renderer::Create(g_d3d_device, 4000);

	// エフェクト管理用インスタンスの生成
	g_manager = ::Effekseer::Manager::Create(4000);

	// 描画用インスタンスから描画機能を設定
	g_manager->SetSpriteRenderer(g_renderer->CreateSpriteRenderer());
	g_manager->SetRibbonRenderer(g_renderer->CreateRibbonRenderer());
	g_manager->SetRingRenderer(g_renderer->CreateRingRenderer());
	g_manager->SetModelRenderer(g_renderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader(g_renderer->CreateTextureLoader());
	g_manager->SetModelLoader(g_renderer->CreateModelLoader());

	// 音再生用インスタンスの生成
	g_sound = ::EffekseerSound::Sound::Create(g_xa2, 16, 16);

	// 音再生用インスタンスから再生機能を指定
	g_manager->SetSoundPlayer(g_sound->CreateSoundPlayer());

	// 音再生用インスタンスからサウンドデータの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetSoundLoader(g_sound->CreateSoundLoader());

	// 視点位置を確定
	g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f));

	// カメラ行列を設定
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// エフェクトの読込
	g_effects[0] = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_ASSETS_DIR_U16 u"r_square.efk");
	g_effects[1] = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_ASSETS_DIR_U16 u"g_square.efk");
	g_effects[2] = Effekseer::Effect::Create(g_manager, EFK_EXAMPLE_ASSETS_DIR_U16 u"b_square.efk");

	MainLoop();

	// エフェクトの破棄
	for (int i = 0; i < 3; i++)
	{
		g_effects[i].Reset();
	}

	// 先にエフェクト管理用インスタンスを破棄
	g_manager.Reset();

	// 次に音再生用インスタンスを破棄
	g_sound.Reset();

	// 次に描画用インスタンスを破棄
	g_renderer.Reset();

	// XAudio2の解放
	if (g_xa2_master != NULL)
	{
		g_xa2_master->DestroyVoice();
		g_xa2_master = NULL;
	}
	ES_SAFE_RELEASE(g_xa2);

	// DirectXの解放
	ES_SAFE_RELEASE(g_d3d_device);
	ES_SAFE_RELEASE(g_d3d);

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