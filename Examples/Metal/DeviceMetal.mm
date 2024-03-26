#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "DeviceMetal.h"

class LLGIWindow : public LLGI::Window
{
	GLFWwindow* window_ = nullptr;

public:
	LLGIWindow(GLFWwindow* window) : window_(window) {}
	
	~LLGIWindow()
	{
		glfwDestroyWindow(window_);
	}
	
	GLFWwindow* GetGLFWwindow()
	{
		return window_;
	}

	bool OnNewFrame() override { return glfwWindowShouldClose(window_) == GL_FALSE; }

	void* GetNativePtr(int32_t index) override
	{
		return glfwGetCocoaWindow(window_);
	}

	LLGI::Vec2I GetWindowSize() const override
	{
		int w, h;
		glfwGetWindowSize(window_, &w, &h);
		return LLGI::Vec2I(w, h);
	}
};

bool DeviceMetal::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto glfwWindow = glfwCreateWindow(windowSize.X, windowSize.Y, windowTitle, nullptr, nullptr);
	if (glfwWindow == nullptr)
	{
		return false;
	}

	window = std::make_shared<LLGIWindow>(glfwWindow);
	if (window == nullptr)
	{
		Terminate();
		return false;
	}

	// A code to initialize Metal is too long, so I use LLGI
	// Metal初期化のためのコードは長すぎるのでLLGIを使用する。

	LLGI::PlatformParameter platformParam{};
	platformParam.Device = LLGI::DeviceType::Metal;
	platformParam.WaitVSync = true;


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
	
	glfwTerminate();
}

bool DeviceMetal::NewFrame()
{
	auto glfwWindow = static_cast<LLGIWindow*>(window.get())->GetGLFWwindow();
	if (glfwWindowShouldClose(glfwWindow))
	{
		return false;
	}

	glfwPollEvents();

	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, glfwGetKey(glfwWindow, key) != GLFW_RELEASE);
	}
	
	if (!platform->NewFrame())
	{
		return false;
	}
	
	memoryPool->NewFrame();

	commandList = commandListPool->Get();

	// Call on starting of a frame
	// フレームの開始時に呼ぶ
	efkMemoryPool->NewFrame();

	// Begin a command list
	// コマンドリストを開始する。
	commandList->Begin();

	EffekseerRendererMetal::BeginCommandList(efkCommandList);
	efkRenderer->SetCommandList(efkCommandList);

	return true;
}

void DeviceMetal::BeginComputePass()
{
	commandList->BeginComputePass();
	EffekseerRendererMetal::BeginComputePass(efkCommandList, GetComputeEncoder());
}

void DeviceMetal::EndComputePass()
{
	EffekseerRendererMetal::EndComputePass(efkCommandList);
	commandList->EndComputePass();
}

void DeviceMetal::BeginRenderPass()
{
	LLGI::Color8 color {0, 0, 0, 255};
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, false));
	EffekseerRendererMetal::BeginRenderPass(efkCommandList, GetRenderEncoder());
}

void DeviceMetal::EndRenderPass()
{
	EffekseerRendererMetal::EndRenderPass(efkCommandList);
	commandList->EndRenderPass();
}

void DeviceMetal::PresentDevice()
{
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererMetal::EndCommandList(efkCommandList);

	// Finish a command list
	// コマンドリストを終了する。
	commandList->End();

	graphics->Execute(commandList);

	platform->Present();
}

void DeviceMetal::SetupEffekseerModules(::Effekseer::ManagerRef efkManager, bool usingProfiler)
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
	
	if (usingProfiler)
	{
		efkManager->SetGpuTimer(efkRenderer->CreateGpuTimer());
	}
}
