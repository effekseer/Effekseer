
#include <functional>
#include <stdio.h>
#include <string>
#include <windows.h>

#include <XAudio2.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "xaudio2.lib")

#include <Effekseer.h>
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>
#include <Window.h>

std::shared_ptr<Utils::Window> g_window;
static LPDIRECT3D9 g_d3d = NULL;
static LPDIRECT3DDEVICE9 g_d3d_device = NULL;

IXAudio2* GetIXAudio2();

bool InitializeWindowAndDevice();
void TerminateWindowAndDevice();
void ClearScreen();
void PresentDevice(const std::function<void()>& onLostDevice, const std::function<void()>& onResetDevice);

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
		auto renderer = ::EffekseerRendererDX9::Renderer::Create(g_d3d_device, 8000);

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

			auto onLostDevice = [renderer, effect]() -> void {
				// Call it before device lost
				// デバイスロストの処理を行う前に実行する
				renderer->OnLostDevice();

				// Dispose all resources in the effect
				// 読み込んだエフェクトのリソースは全て破棄する。
				if (effect != nullptr)
				{
					effect->UnloadResources();
				}
			};

			auto onResetDevice = [renderer, effect]() -> void {
				// Reload all resources in the effect
				// エフェクトのリソースを再読み込みする。
				if (effect != nullptr)
				{
					effect->ReloadResources();
				}

				// Call it after device lost
				// デバイスロストの処理の後に実行する
				renderer->OnResetDevice();
			};

			PresentDevice(onLostDevice, onResetDevice);

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

static IXAudio2* g_xa2 = nullptr;
static IXAudio2MasteringVoice* g_xa2_master = nullptr;

IXAudio2* GetIXAudio2() { return g_xa2; }

bool InitializeWindowAndDevice()
{
	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// Initialize Window
	// ウインドウの初期化
	g_window = Utils::Window::Create("DirectX9", Utils::Vec2I{1280, 720});

	// Initialize DirectX9
	// DirectX9の初期化
	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = g_window->GetWindowSize().X;
	d3dp.BackBufferHeight = g_window->GetWindowSize().Y;
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = (HWND)g_window->GetNativePtr(0);
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	g_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	g_d3d->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)g_window->GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &g_d3d_device);

	// XAudio2の初期化を行う
	XAudio2Create(&g_xa2);

	g_xa2->CreateMasteringVoice(&g_xa2_master);

	return true;
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
	ES_SAFE_RELEASE(g_d3d_device);
	ES_SAFE_RELEASE(g_d3d);

	g_window = nullptr;

	// Release COM
	// COMの解放
	CoUninitialize();
}

void ClearScreen()
{
	g_d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	g_d3d_device->BeginScene();
}

void PresentDevice(const std::function<void()>& onLostDevice, const std::function<void()>& onResetDevice)
{
	g_d3d_device->EndScene();

	{
		HRESULT hr;
		hr = g_d3d_device->Present(NULL, NULL, NULL, NULL);

		switch (hr)
		{
		case D3DERR_DEVICELOST:
			while (FAILED(hr = g_d3d_device->TestCooperativeLevel()))
			{
				switch (hr)
				{
				case D3DERR_DEVICELOST:
					::SleepEx(1000, true);
					break;

				case D3DERR_DEVICENOTRESET:

					// Call it before device lost
					// デバイスロストの処理を行う前に実行する
					onLostDevice();
					D3DPRESENT_PARAMETERS d3dp;
					ZeroMemory(&d3dp, sizeof(d3dp));
					d3dp.BackBufferWidth = g_window->GetWindowSize().X;
					d3dp.BackBufferHeight = g_window->GetWindowSize().Y;
					d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
					d3dp.BackBufferCount = 1;
					d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
					d3dp.Windowed = TRUE;
					d3dp.hDeviceWindow = (HWND)g_window->GetNativePtr(0);
					d3dp.EnableAutoDepthStencil = TRUE;
					d3dp.AutoDepthStencilFormat = D3DFMT_D16;

					g_d3d_device->Reset(&d3dp);

					// Call it after device lost
					// デバイスロストの処理の後に実行する
					onResetDevice();

					break;
				}
			}
			break;
		}
	}
}
