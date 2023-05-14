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
	device.Initialize("NetworkServer (OpenGL)", screenSize);
#elif defined(DEVICE_DX9)
	DeviceDX9 device;
	device.Initialize("NetworkServer (DirectX9)", screenSize);
#elif defined(DEVICE_DX11)
	DeviceDX11 device;
	device.Initialize("NetworkServer (DirectX11)", screenSize);
#endif

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

	// Create a server for effect
	// サーバーの作成
	auto efkServer = Effekseer::Server::Create();

	// Start the server on port 60000
	// サーバーをポート60000で開始
	efkServer->Start(60000);

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

	// Register the effect asset to server for reloading
	// エフェクトをリロードするためのサーバーへ登録
	efkServer->Register(u"Laser01", effect);

	int32_t time = 0;
	Effekseer::Handle efkHandle = 0;

	while (device.NewFrame())
	{
		// Update the server
		// サーバーの更新を行う。
		efkServer->Update(&efkManager, 1);

		if (time % 120 == 0)
		{
			// Play an effect
			// エフェクトの再生
			efkHandle = efkManager->Play(effect, 0, 0, 0);
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

		// Execute functions about Rendering
		// 描画の処理
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
