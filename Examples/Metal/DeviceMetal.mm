#include "DeviceMetal.h"

bool DeviceMetal::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	// A code to initialize DirectX12 is too long, so I use LLGI
	// Metal初期化のためのコードは長すぎるのでLLGIを使用する。

	LLGI::PlatformParameter platformParam{};
	platformParam.Device = LLGI::DeviceType::Metal;
	platformParam.WaitVSync = true;

	window = std::shared_ptr<LLGI::Window>(LLGI::CreateWindow(windowTitle, { windowSize.X, windowSize.Y }));
	if (window == nullptr)
	{
		Terminate();
		return false;
	}

	platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(platformParam, window.get()));

	if (platform == nullptr)
	{
		Terminate();
		return false;
	}

	graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());

	if (graphics == nullptr)
	{
		Terminate();
		return false;
	}

	memoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));
	commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), memoryPool.get(), 3);

	return true;
}

void DeviceMetal::Terminate()
{
	// Release Metal
	// Metalの解放

	if (graphics)
	{
		graphics->WaitFinish();
	}

	efkCommandList.Reset();
	efkMemoryPool.Reset();
	efkRenderer.Reset();

	commandListPool.reset();
	memoryPool.reset();
	graphics.reset();
	platform.reset();
	window.reset();
}

void DeviceMetal::ClearScreen()
{
}

bool DeviceMetal::NewFrame()
{
	if (!platform->NewFrame())
		return false;

	memoryPool->NewFrame();

	commandList = commandListPool->Get();

	LLGI::Color8 color;
	color.R = 0;
	color.G = 0;
	color.B = 0;
	color.A = 255;

	commandList->Begin();
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, false));

	// Call on starting of a frame
	// フレームの開始時に呼ぶ
	efkMemoryPool->NewFrame();

	// Begin a command list
	// コマンドリストを開始する。
	EffekseerRendererMetal::BeginCommandList(efkCommandList, GetEncoder());
	efkRenderer->SetCommandList(efkCommandList);

	return true;
}

void DeviceMetal::PresentDevice()
{
	// Finish a command list
	// コマンドリストを終了する。
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererMetal::EndCommandList(efkCommandList);

	commandList->EndRenderPass();
	commandList->End();

	graphics->Execute(commandList);

	platform->Present();
}

void DeviceMetal::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	efkRenderer = ::EffekseerRendererMetal::Create(
		8000, MTLPixelFormatBGRA8Unorm, MTLPixelFormatInvalid, false);
	
	// Create a memory pool
	// メモリプールの作成
	efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Create a command list
	// コマンドリストの作成
	efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);

	// Sprcify rendering modules
	// 描画モジュールの設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
}
