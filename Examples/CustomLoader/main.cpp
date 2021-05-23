
#include <stdio.h>
#include <string>

#include <Effekseer.Modules.h>
#include <Effekseer.h>
#include <EffekseerRendererGL.h>

// A helper function to load an image
// イメージを読み込むためのヘルパー関数
#define STB_IMAGE_EFK_IMPLEMENTATION
#include <stb_image_utils.h>

bool InitializeWindowAndDevice(int32_t windowWidth, int32_t windowHeight);
bool DoEvent();
void TerminateWindowAndDevice();
void ClearScreen();
void PresentDevice();

/**
Effekseer supports basic loading, but it is not designed to be loadable from other game engine packages.
So add a loader class and extend the file loading method.

Effekseerでは基本的な読込はサポートしていますが、他のゲームエンジンのパッケージから読み込めるようになっていません。
そのため、ローダークラスを追加し、ファイルの読込方法を拡張します。
*/

class CustomTextureLoader : public ::Effekseer::TextureLoader
{
private:
	::Effekseer::FileInterface* fileInterface_;
	::Effekseer::DefaultFileInterface defaultFileInterface_;
	::Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;

public:
	// FileInterface is a binary loader. A behavior can be changed with a inheritance
	// FileInterfaceはバイナリローダーです。振る舞いを継承により変更できます。
	CustomTextureLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface = nullptr)
		: fileInterface_(fileInterface)
	{
		graphicsDevice_ = graphicsDevice;

		if (fileInterface_ == nullptr)
		{
			fileInterface_ = &defaultFileInterface_;
		}
	}
	virtual ~CustomTextureLoader() = default;

public:
	Effekseer::TextureRef Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType) override
	{
		std::unique_ptr<::Effekseer::FileReader> reader(fileInterface_->OpenRead(path));

		if (reader.get() != nullptr)
		{
			size_t size_texture = reader->GetLength();
			std::vector<char> data_texture;
			data_texture.resize(size_texture);
			reader->Read(data_texture.data(), size_texture);

			// load an image with the helper function
			// ヘルパー関数で画像を読み込む
			int width;
			int height;
			int bpp;
			uint8_t* pixels = (uint8_t*)EffekseerUtils::stbi_load_from_memory(
				(EffekseerUtils::stbi_uc const*)data_texture.data(), static_cast<int>(size_texture), &width, &height, &bpp, 0);

			if (width == 0 || bpp < 3)
			{
				// Not supported
				EffekseerUtils::stbi_image_free(pixels);
				return nullptr;
			}

			// Load a image to GPU actually. Please see a code of each backends if you want to know what should be returned
			// 実際にGPUに画像を読み込む。何を返すべきか知りたい場合、、各バックエンドのコードを読んでください。
			::Effekseer::Backend::TextureParameter param;
			param.Format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
			param.GenerateMipmap = true;
			param.Size[0] = width;
			param.Size[1] = height;

			if (bpp == 4)
			{
				param.InitialData.assign(pixels, pixels + width * height * 4);
			}
			else
			{
				param.InitialData.resize(width * height * 4);
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						param.InitialData[(x + y * width) * 4 + 0] = pixels[(x + y * width) * 3 + 0];
						param.InitialData[(x + y * width) * 4 + 1] = pixels[(x + y * width) * 3 + 1];
						param.InitialData[(x + y * width) * 4 + 2] = pixels[(x + y * width) * 3 + 2];
						param.InitialData[(x + y * width) * 4 + 3] = 255;
					}
				}
			}

			auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
			texture->SetBackend(graphicsDevice_->CreateTexture(param));
			return texture;
		}

		return nullptr;
	}

	void Unload(Effekseer::TextureRef data) override
	{
		// Do nothing
	}
};

class CustomModelLoader : public ::Effekseer::ModelLoader
{
private:
	::Effekseer::DefaultFileInterface defaultFileInterface_;
	::Effekseer::FileInterface* fileInterface_;

public:
	CustomModelLoader(::Effekseer::FileInterface* fileInterface = nullptr) : fileInterface_(fileInterface)
	{
		if (fileInterface == nullptr)
		{
			fileInterface_ = &defaultFileInterface_;
		}
	}

	~CustomModelLoader() override = default;

public:
	Effekseer::ModelRef Load(const char16_t* path) override
	{
		std::unique_ptr<::Effekseer::FileReader> reader(fileInterface_->OpenRead(path));
		if (reader.get() == nullptr)
		{
			return nullptr;
		}

		size_t size = reader->GetLength();
		std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
		reader->Read(data.get(), size);

		auto model = Load(data.get(), (int32_t)size);

		return model;
	}

	Effekseer::ModelRef Load(const void* data, int32_t size) override
	{
		auto model = ::Effekseer::MakeRefPtr<::Effekseer::Model>((const uint8_t*)data, size);

		return model;
	}

	void Unload(Effekseer::ModelRef data) override {}
};

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

	// Specify a texture, model and material loader
	// The texture and model loaders are extended by yourself.
	// テクスチャ、モデル、マテリアルローダーの設定する。
	// テクスチャとモデルローダーが拡張されている。
	manager->SetTextureLoader(::Effekseer::TextureLoaderRef(new CustomTextureLoader(renderer->GetGraphicsDevice())));
	manager->SetModelLoader(::Effekseer::MakeRefPtr<CustomModelLoader>());


	// You can specify only a file loader
	// ファイルローダーのみを指定することもできる。
	::Effekseer::DefaultFileInterface fileInterface;
	manager->SetMaterialLoader(renderer->CreateMaterialLoader(&fileInterface));

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