
#include <stdio.h>
#include <string>
#include <windows.h>

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include <EffekseerSoundXAudio2.h>
#include "DeviceDX12.h"

int main(int argc, char** argv)
{
	DeviceDX12 device;
	device.Initialize("DirectX12", Utils::Vec2I{1280, 720});

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	auto efkRenderer = ::EffekseerRendererDX12::Create(device.GetID3D12Device(), device.GetCommandQueue(), 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	// Create a memory pool
	// メモリプールの作成
	auto sfMemoryPoolEfk = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Create a command list
	// コマンドリストの作成
	auto commandListEfk = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), sfMemoryPoolEfk);

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

	// Specify sound modules
	// サウンドモジュールの設定
	auto efkSound = ::EffekseerSound::Sound::Create(device.GetIXAudio2(), 16, 16);

	// Specify a metho to play sound from an instance of efkSound
	// 音再生用インスタンスから再生機能を指定
	efkManager->SetSoundPlayer(efkSound->CreateSoundPlayer());

	// Specify a sound data loader
	// It can be extended by yourself. It is loaded from a file on now.
	// サウンドデータの読込機能を設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetSoundLoader(efkSound->CreateSoundLoader());

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

	int32_t time = 0;
	Effekseer::Handle efkHandle = 0;

	while (device.NewFrame())
	{
		// Call on starting of a frame
		// フレームの開始時に呼ぶ
		sfMemoryPoolEfk->NewFrame();

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

		// Begin a command list
		// コマンドリストを開始する。
		EffekseerRendererDX12::BeginCommandList(commandListEfk, device.GetCommandList());
		efkRenderer->SetCommandList(commandListEfk);

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

		// Finish a command list
		// コマンドリストを終了する。
		efkRenderer->SetCommandList(nullptr);
		EffekseerRendererDX12::EndCommandList(commandListEfk);

		// Execute functions about DirectX
		// DirectXの処理
		device.PresentDevice();

		time++;
	}

	// Dispose the manager
	// マネージャーの破棄
	efkManager.Reset();

	// Dispose the sound
	// サウンドの破棄
	efkSound.Reset();

	// Dispose the renderer
	// レンダラーの破棄
	efkRenderer.Reset();

	commandListEfk.Reset();

	return 0;
}
