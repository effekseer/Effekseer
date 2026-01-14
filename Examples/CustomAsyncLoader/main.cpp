// Choose from the following graphics APIs you want to enable
// グラフィックスAPIを下記から選んで有効にしてください
#define DEVICE_OPENGL
// #define DEVICE_DX9
// #define DEVICE_DX11

#include <map>
#include <stdio.h>
#include <string>

#include "CustomAsyncLoader.h"

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

int main(int argc, char** argv)
{
#if defined(DEVICE_OPENGL)
	DeviceGLFW device;
	device.Initialize("AsyncLoader (OpenGL)", screenSize);
#elif defined(DEVICE_DX9)
	DeviceDX9 device;
	device.Initialize("AsyncLoader (DirectX9)", screenSize);
#elif defined(DEVICE_DX11)
	DeviceDX11 device;
	device.Initialize("AsyncLoader (DirectX11)", screenSize);
#endif

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

	// Sprcify rendering modules
	// 描画モジュールの設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	auto setting = Effekseer::Setting::Create();

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	setting->SetTextureLoader(efkRenderer->CreateTextureLoader());
	setting->SetModelLoader(efkRenderer->CreateModelLoader());
	setting->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	setting->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	setting->ClearEffectFactory();
	setting->AddEffectFactory(Effekseer::MakeRefPtr<CustomEfkEfcFactory>());

	auto fileInterface = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	auto asyncLoader = CustomAsyncLoader::Create(fileInterface, setting);

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

	int32_t time = 0;

	CustomAsyncValueHandle<Effekseer::Effect> effectHandle(nullptr);

	while (device.NewFrame())
	{
		asyncLoader->Update();

		if (time % 120 == 0)
		{
			effectHandle = asyncLoader->LoadEffectAsync(EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efkefc");
		}

		if (effectHandle.GetIsValid() && effectHandle.GetState() == AsyncLoadingState::Finished)
		{
			efkManager->Play(effectHandle.GetValue(), 0, 0, 0);
			effectHandle = CustomAsyncValueHandle<Effekseer::Effect>(nullptr);
		}

		// Set layer parameters
		// レイヤーパラメータの設定
		Effekseer::Manager::LayerParameter layerParameter;
		layerParameter.ViewerPosition = viewerPosition;
		efkManager->SetLayerParameter(0, layerParameter);

		// Update the manager
		// マネージャーの更新
		Effekseer::Manager::UpdateParameter updateParameter;
		efkManager->Update(updateParameter);

		// Begin to rendering pass
		// 描画パスの開始
		device.BeginRenderPass();

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

		// Finish to rendering pass
		// 描画パスの終了
		device.EndRenderPass();

		// Update the display
		// ディスプレイを更新
		device.PresentDevice();

		time++;
	}

	return 0;
}
