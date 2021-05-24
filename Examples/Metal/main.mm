
#include <stdio.h>
#include <string>

#include <Effekseer.h>
#include <EffekseerRendererMetal.h>
#include <Window.h>

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);
void TerminateWindowAndDevice();
bool BeginFrame();
void EndFrame();

int GetSwapBufferCount();

id<MTLRenderCommandEncoder> GetEncoder();

int main(int argc, char** argv)
{
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;

	InitializeWindowAndDevice(windowWidth, windowHeight);

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
    auto renderer = ::EffekseerRendererMetal::Create(
		8000, MTLPixelFormatBGRA8Unorm, MTLPixelFormatInvalid, false);

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
		EffekseerRendererMetal::BeginCommandList(commandListEfk, GetEncoder());
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
		EffekseerRendererMetal::EndCommandList(commandListEfk);

		// Ececute functions about DirectX
		// DirectXの処理
		EndFrame();

		time++;
	}

	// Dispose the manager
	// マネージャーの破棄
	manager.Reset();

	// Dispose the renderer
	// レンダラーの破棄
	renderer.Reset();

	TerminateWindowAndDevice();

	return 0;
}

#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Utils/LLGI.CommandListPool.h>
#include <Metal/LLGI.CommandListMetal.h>
#include <Metal/LLGI.GraphicsMetal.h>
#include <Metal/LLGI.Metal_Impl.h>

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

id<MTLRenderCommandEncoder> GetEncoder()
{
    return static_cast<LLGI::CommandListMetal*>(context->commandList)->GetRenderCommandEncorder();
}

int GetSwapBufferCount() { return 3; }

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight)
{
	context = std::make_shared<ContextLLGI>();

	// A code to initialize Metal is too long, so I use LLGI
	// Metal初期化のためのコードは長すぎるのでLLGIを使用する。

	LLGI::PlatformParameter platformParam;
	platformParam.Device = LLGI::DeviceType::Metal;
	platformParam.WaitVSync = true;
    
	context->window = std::shared_ptr<LLGI::Window>(LLGI::CreateWindow("Metal", LLGI::Vec2I(windowWidth, windowHeight)));
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

	return true;
FAILED:
	return false;
}

void TerminateWindowAndDevice()
{
	// Release Metal
	// Metalの解放
	context->graphics->WaitFinish();
	context = nullptr;
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
		context->platform->GetCurrentScreen(color, true, true));

	return true;
}

void EndFrame()
{
	context->commandList->EndRenderPass();
	context->commandList->End();

	context->graphics->Execute(context->commandList);

	context->platform->Present();
}
