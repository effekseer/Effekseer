
#include <stdio.h>
#include <string>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);
bool DoEvent();
void TerminateWindowAndDevice();
void ClearScreen();
void PresentDevice();

int main(int argc, char** argv)
{
	int32_t windowWidth = 1280;
	int32_t windowHeight = 720;
	InitializeWindowAndDevice(windowWidth, windowHeight);

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	auto renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

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
		::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, (float)windowWidth / (float)windowHeight, 1.0f, 500.0f));

	// Specify a camera matrix
	// カメラ行列を設定
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// Load an effect
	// エフェクトの読込
	auto effect = Effekseer::Effect::Create(manager, EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efk");

	int32_t time = 0;
	Effekseer::Handle handle = 0;

	while (DoEvent())
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

	// Dispose the renderer
	// レンダラーの破棄
	renderer.Reset();

	TerminateWindowAndDevice();

	return 0;
}

// I use glfw to easy to write
// 簡単に書くために、glfwを使用します。
// https://www.glfw.org/
#include <GLFW/glfw3.h>
static GLFWwindow* glfwWindow = nullptr;

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight)
{
	// Initialize Window
	// ウインドウの初期化
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

#if !_WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);
	return true;
}

bool DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

void TerminateWindowAndDevice()
{
	if (glfwWindow != nullptr)
	{
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
		glfwWindow = nullptr;
	}
}

void ClearScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PresentDevice() { glfwSwapBuffers(glfwWindow); }