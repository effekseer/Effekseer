// Choose from the following graphics APIs you want to enable
// グラフィックスAPIを下記から選んで有効にしてください
#define DEVICE_OPENGL
//#define DEVICE_DX9
//#define DEVICE_DX11

#include <stdio.h>
#include <string>

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
	device.Initialize("Trigger - Press the \"Space\" key (OpenGL)", screenSize);
#elif defined(DEVICE_DX9)
	DeviceDX9 device;
	device.Initialize("Trigger - Press the \"Space\" key (DirectX9)", screenSize);
#elif defined(DEVICE_DX11)
	DeviceDX11 device;
	device.Initialize("Trigger - Press the \"Space\" key (DirectX11)", screenSize);
#endif

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(2000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

	// Specify a position of view
	// 視点位置を確定
	auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f) * 0.5f;

	// Specify a projection matrix
	// 投影行列を設定
	::Effekseer::Matrix44 projectionMatrix;
	projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

	// Specify a camera matrix
	// カメラ行列を設定
	::Effekseer::Matrix44 cameraMatrix;
	cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

	auto effect = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"TriggerLaser.efkefc");

	Effekseer::Handle handle = 0;
	int32_t time = 0;

	while (device.NewFrame())
	{
		if (efkManager->Exists(handle))
		{
			// Press the "Space" key
			// スペースキーを押す
			if (Utils::Input::IsKeyPressed(' '))
			{
				// Send a trigger-0 to effect
				// エフェクトにトリガー0を送信する
				efkManager->SendTrigger(handle, 0);
			}
		}
		else
		{
			// Play an effect
			// エフェクトの再生
			handle = efkManager->Play(effect, 0, 0, 0);
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

		// Clear render target buffer
		// レンダリングターゲットをクリア
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

		// Render rear effects
		// エフェクトの描画を行う。
		Effekseer::Manager::DrawParameter drawParameter;
		drawParameter.ZNear = 0.0f;
		drawParameter.ZFar = 1.0f;
		drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
		efkManager->Draw(drawParameter);

		// Finish to rendering effects
		// エフェクトの描画終了処理を行う。
		efkRenderer->EndRendering();

		// Update the display
		// ディスプレイを更新
		device.PresentDevice();

		time++;
	}

	// Stop effects
	// エフェクトの停止
	efkManager->StopEffect(handle);

	return 0;
}
