
#include <stdio.h>
#include <string>
#include <windows.h>

#include <XAudio2.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "xaudio2.lib")

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <EffekseerSoundXAudio2.h>
#include <Window.h>

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);
void TerminateWindowAndDevice();
bool BeginFrame();
void EndFrame();

IXAudio2* GetIXAudio2();

ID3D12Device* GetDevice();

ID3D12CommandQueue* GetCommandQueue();

ID3D12GraphicsCommandList* GetCommandList();

int main(int argc, char** argv)
{
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;

	InitializeWindowAndDevice(windowWidth, windowHeight);

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// However, if it is disposed before the end of COM, it will go wrong, so we add a scope.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。
	// ただし、COMの終了前に削除されるとおかしくなるので、スコープを追加する。
	{
		// Create a renderer of effects
		// エフェクトのレンダラーの作成
		auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
		auto renderer = ::EffekseerRendererDX12::Create(GetDevice(), GetCommandQueue(), 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

		// Create a memory pool
		// メモリプールの作成
		auto sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());

		// Create a command list
		// コマンドリストの作成
		auto commandListEfk = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), sfMemoryPoolEfk);

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
		renderer->SetProjectionMatrix(
			::Effekseer::Matrix44().PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)windowWidth / (float)windowHeight, 1.0f, 500.0f));

		// Specify a camera matrix
		// カメラ行列を設定
		renderer->SetCameraMatrix(
			::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

		// Load an effect
		// エフェクトの読込
		auto effect = Effekseer::Effect::Create(manager, EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efk");

		int32_t time = 0;
		Effekseer::Handle handle = 0;

		while (BeginFrame())
		{
			// Call on starting of a frame
			// フレームの開始時に呼ぶ
			sfMemoryPoolEfk->NewFrame();

			// Begin a command list
			// コマンドリストを開始する。
			EffekseerRendererDX12::BeginCommandList(commandListEfk, GetCommandList());
			renderer->SetCommandList(commandListEfk);

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

			// Finish a command list
			// コマンドリストを終了する。
			renderer->SetCommandList(nullptr);
			EffekseerRendererDX12::EndCommandList(commandListEfk);

			// Ececute functions about DirectX
			// DirectXの処理
			EndFrame();

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

#include <DX12/LLGI.CommandListDX12.h>
#include <DX12/LLGI.GraphicsDX12.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Utils/LLGI.CommandListPool.h>

struct ContextLLGI
{
	std::shared_ptr<LLGI::Window> window;
	std::shared_ptr<LLGI::Platform> platform;
	std::shared_ptr<LLGI::Graphics> graphics;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> memoryPool;
	std::shared_ptr<LLGI::CommandListPool> commandListPool;
	LLGI::CommandList* commandList = nullptr;
};

std::shared_ptr<ContextLLGI> context;

ID3D12Device* GetDevice()
{
	assert(context != nullptr);
	assert(context->graphics != nullptr);

	return static_cast<LLGI::GraphicsDX12*>(context->graphics.get())->GetDevice();
}

ID3D12CommandQueue* GetCommandQueue()
{
	assert(context != nullptr);
	assert(context->graphics != nullptr);
	return static_cast<LLGI::GraphicsDX12*>(context->graphics.get())->GetCommandQueue();
}

ID3D12GraphicsCommandList* GetCommandList()
{
	assert(context != nullptr);
	assert(context->commandList != nullptr);
	return static_cast<LLGI::CommandListDX12*>(context->commandList)->GetCommandList();
}

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight)
{
	context = std::make_shared<ContextLLGI>();

	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// A code to initialize DirectX12 is too long, so I use LLGI
	// DirectX12初期化のためのコードは長すぎるのでLLGIを使用する。
	LLGI::PlatformParameter platformParam{};
	platformParam.Device = LLGI::DeviceType::DirectX12;
	platformParam.WaitVSync = true;

	context->window = std::shared_ptr<LLGI::Window>(LLGI::CreateWindow("DirectX12", LLGI::Vec2I(windowWidth, windowHeight)));
	if (context->window == nullptr)
	{
		goto FAILED;
	}

	context->platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(platformParam, context->window.get()));

	if (context->platform == nullptr)
	{
		goto FAILED;
	}

	context->graphics = LLGI::CreateSharedPtr(context->platform->CreateGraphics());

	if (context->graphics == nullptr)
	{
		goto FAILED;
	}

	context->memoryPool = LLGI::CreateSharedPtr(context->graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));
	context->commandListPool = std::make_shared<LLGI::CommandListPool>(context->graphics.get(), context->memoryPool.get(), 3);

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(&g_xa2);

	g_xa2->CreateMasteringVoice(&g_xa2_master);

	return true;
FAILED:
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
	context->graphics->WaitFinish();
	context = nullptr;

	// Release COM
	// COMの解放
	CoUninitialize();
}

bool BeginFrame()
{

	if (!context->platform->NewFrame())
		return false;

	context->memoryPool->NewFrame();

	context->commandList = context->commandListPool->Get();

	LLGI::Color8 color;
	color.R = 0;
	color.G = 0;
	color.B = 0;
	color.A = 255;

	context->commandList->Begin();
	context->commandList->BeginRenderPass(
		context->platform->GetCurrentScreen(color, true, false)); // TODO: isDepthClear is false, because it fails with dx12.

	return true;
}

void EndFrame()
{
	context->commandList->EndRenderPass();
	context->commandList->End();

	context->graphics->Execute(context->commandList);

	context->platform->Present();
}
