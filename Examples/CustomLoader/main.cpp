// Choose from the following graphics APIs you want to enable
// グラフィックスAPIを下記から選んで有効にしてください
#define DEVICE_OPENGL
//#define DEVICE_DX9
//#define DEVICE_DX11

#include <stdio.h>
#include <string>

#include <Effekseer.Modules.h>
#include <Effekseer.h>

#if defined(DEVICE_OPENGL)
#include "../OpenGL/DeviceGLFW.h"
#elif defined(DEVICE_DX9)
#include "../DirectX9/DeviceDX9.h"
#elif defined(DEVICE_DX11)
#include "../DirectX11/DeviceDX11.h"
#endif

const Utils::Vec2I screenSize = {1280, 720};

// A helper function to load an image
// イメージを読み込むためのヘルパー関数
#define STB_IMAGE_EFK_IMPLEMENTATION
#include <stb_image_utils.h>

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
	Effekseer::TextureRef Load(const char16_t* path, ::Effekseer::TextureType textureType) override
	{
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
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
			param.MipLevelCount = 1;
			param.Size = {width, height, 0};
			param.Dimension = 2;
			Effekseer::CustomVector<uint8_t> initialData;

			if (bpp == 4)
			{
				initialData.assign(pixels, pixels + width * height * 4);
			}
			else
			{
				initialData.resize(width * height * 4);
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width; x++)
					{
						initialData[(x + y * width) * 4 + 0] = pixels[(x + y * width) * 3 + 0];
						initialData[(x + y * width) * 4 + 1] = pixels[(x + y * width) * 3 + 1];
						initialData[(x + y * width) * 4 + 2] = pixels[(x + y * width) * 3 + 2];
						initialData[(x + y * width) * 4 + 3] = 255;
					}
				}
			}

			auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
			texture->SetBackend(graphicsDevice_->CreateTexture(param, initialData));
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
	CustomModelLoader(::Effekseer::FileInterface* fileInterface = nullptr)
		: fileInterface_(fileInterface)
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
		auto reader = fileInterface_->OpenRead(path);
		if (reader == nullptr)
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

	void Unload(Effekseer::ModelRef data) override
	{
	}
};

int main(int argc, char** argv)
{
#if defined(DEVICE_OPENGL)
	DeviceGLFW device;
	device.Initialize("CustomLoader (OpenGL)", screenSize);
#elif defined(DEVICE_DX9)
	DeviceDX9 device;
	device.Initialize("CustomLoader (DirectX9)", screenSize);
#elif defined(DEVICE_DX11)
	DeviceDX11 device;
	device.Initialize("CustomLoader (DirectX11)", screenSize);
#endif

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

	// Specify a texture, model and material loader
	// The texture and model loaders are extended by yourself.
	// テクスチャ、モデル、マテリアルローダーの設定する。
	// テクスチャとモデルローダーが拡張されている。
	efkManager->SetTextureLoader(::Effekseer::MakeRefPtr<CustomTextureLoader>(efkRenderer->GetGraphicsDevice()));
	efkManager->SetModelLoader(::Effekseer::MakeRefPtr<CustomModelLoader>());

	// Specify a position of view
	// 視点位置を確定
	auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// Specify a projection matrix
	// 投影行列を設定
	::Effekseer::Matrix44 projectionMatrix;
	projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

	// Specify a camera matrix
	// カメラ行列を設定
	::Effekseer::Matrix44 cameraMatrix;
	cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

	// Load an effect
	// エフェクトの読込
	auto effect = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efkefc");

	int32_t time = 0;
	Effekseer::Handle efkHandle = 0;

	while (device.NewFrame())
	{
		if (time % 120 == 0)
		{
			// Play an effect
			// エフェクトの再生
			efkHandle = efkManager->Play(effect, 0, 0, 0);
		}

		if (time % 120 == 119)
		{
			// Stop effects
			// エフェクトの停止
			efkManager->StopEffect(efkHandle);
		}

		// Move the effect
		// エフェクトの移動
		efkManager->AddLocation(efkHandle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));

		// Set layer parameters
		// レイヤーパラメータの設定
		Effekseer::Manager::LayerParameter layerParameter;
		layerParameter.ViewerPosition = viewerPosition;
		efkManager->SetLayerParameter(0, layerParameter);

		// Update the manager
		// マネージャーの更新
		Effekseer::Manager::UpdateParameter updateParameter;
		efkManager->Update(updateParameter);

		// Execute functions about DirectX
		// DirectXの処理
		device.ClearScreen();

		// Update a time
		// 時間を更新する
		efkRenderer->SetTime(time / 60.0f);

		// Specify a projection matrix
		// 投影行列を設定
		efkRenderer->SetProjectionMatrix(projectionMatrix);

		// Specify a camera matrix
		// カメラ行列を設定
		efkRenderer->SetCameraMatrix(cameraMatrix);

		// Begin to rendering effects
		// エフェクトの描画開始処理を行う。
		efkRenderer->BeginRendering();

		// Render effects
		// エフェクトの描画を行う。
		Effekseer::Manager::DrawParameter drawParameter;
		drawParameter.ZNear = 0.0f;
		drawParameter.ZFar = 1.0f;
		drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
		efkManager->Draw(drawParameter);

		// Finish to rendering effects
		// エフェクトの描画終了処理を行う。
		efkRenderer->EndRendering();

		// Execute functions about DirectX
		// DirectXの処理
		device.PresentDevice();

		time++;
	}

	return 0;
}
