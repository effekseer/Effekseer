#include "DeviceGLFW.h"

Utils::Vec2I DeviceGLFW::GetWindowSize() const
{
	int width = 0, height = 0;
	glfwGetWindowSize(glfwWindow, &width, &height);
	return {width, height};
}

bool DeviceGLFW::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	// Initialize Window
	// ウインドウの初期化
	if (!glfwInit())
	{
		Terminate();
		return false;
	}

	if (deviceType == ::EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	glfwWindow = glfwCreateWindow(windowSize.X, windowSize.Y, windowTitle, nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		Terminate();
		return false;
	}

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	return true;
}

void DeviceGLFW::Terminate()
{
	efkRenderer.Reset();

	if (glfwWindow != nullptr)
	{
		glfwDestroyWindow(glfwWindow);
		glfwWindow = nullptr;
		glfwTerminate();
	}
}

void DeviceGLFW::ClearScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool DeviceGLFW::NewFrame()
{
	if (glfwWindowShouldClose(glfwWindow))
	{
		return false;
	}

	glfwPollEvents();

	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, glfwGetKey(glfwWindow, key) != GLFW_RELEASE);
	}

	return true;
}

void DeviceGLFW::PresentDevice()
{
	glfwSwapBuffers(glfwWindow);
}

void DeviceGLFW::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererGL::CreateGraphicsDevice(deviceType);

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	efkRenderer = ::EffekseerRendererGL::Renderer::Create(graphicsDevice, 8000);

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

#if USE_OPENAL
	// Specify sound modules
	// サウンドモジュールの設定
	efkSound = ::EffekseerSound::Sound::Create(16, 16);

	// Specify a metho to play sound from an instance of efkSound
	// 音再生用インスタンスから再生機能を指定
	efkManager->SetSoundPlayer(efkSound->CreateSoundPlayer());

	// Specify a sound data loader
	// It can be extended by yourself. It is loaded from a file on now.
	// サウンドデータの読込機能を設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetSoundLoader(efkSound->CreateSoundLoader());
#endif
}
