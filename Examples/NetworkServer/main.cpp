
#include <stdio.h>
#include <string>
#include <windows.h>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include "../Utils/Windows/Window.h"
#include "../OpenGL/DeviceGLFW.h"

int main(int argc, char** argv)
{
	DeviceGLFW device;
	device.Initialize("NetworkServer", Utils::Vec2I{1280, 720});

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	auto efkRenderer = ::EffekseerRendererGL::Renderer::Create(8000,
		::EffekseerRendererGL::OpenGLDeviceType::OpenGL3, true);

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

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
	efkRenderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH(
		90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f));

	// Specify a camera matrix
	// カメラ行列を設定
	efkRenderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

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

	// Dispose the server
	// サーバーの破棄
	efkServer.Reset();

	// Dispose the manager
	// マネージャーの破棄
	efkManager.Reset();

	// Dispose the renderer
	// レンダラーの破棄
	efkRenderer.Reset();

	return 0;
}
