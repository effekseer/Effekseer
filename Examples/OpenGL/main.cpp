
#include <stdio.h>
#include <string>

#include "DeviceGLFW.h"
#include <Effekseer.h>
#include <EffekseerRendererGL.h>

int main(int argc, char** argv)
{
	DeviceGLFW device;
	device.Initialize("OpenGL", Utils::Vec2I{1280, 720});

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

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

	efkRenderer.Reset();
	efkManager.Reset();

	return 0;
}
