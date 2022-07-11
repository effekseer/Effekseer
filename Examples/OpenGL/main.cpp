﻿
#include <stdio.h>
#include <string>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include "DeviceGLFW.h"

int main(int argc, char** argv)
{
	DeviceGLFW device;
	device.Initialize("NetworkServer", { 1280, 720 });

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
	auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// Specify a projection matrix
	// 投影行列を設定
	renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, 
		(float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f));

	// Specify a camera matrix
	// カメラ行列を設定
	renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// Load an effect
	// エフェクトの読込
	auto effect = Effekseer::Effect::Create(manager, EFK_EXAMPLE_ASSETS_DIR_U16 "Laser01.efk");

	int32_t time = 0;
	Effekseer::Handle handle = 0;

	while (device.NewFrame())
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

		// Set layer parameters
		// レイヤーパラメータの設定
		Effekseer::Manager::LayerParameter layerParameter;
		layerParameter.ViewerPosition = viewerPosition;
		manager->SetLayerParameter(0, layerParameter);

		// Update the manager
		// マネージャーの更新
		Effekseer::Manager::UpdateParameter updateParameter;
		manager->Update(updateParameter);

		// Execute functions about OpenGL
		// OpenGLの処理
		device.ClearScreen();

		// Begin to rendering effects
		// エフェクトの描画開始処理を行う。
		renderer->BeginRendering();

		// Render effects
		// エフェクトの描画を行う。
		Effekseer::Manager::DrawParameter drawParameter;
		drawParameter.ZNear = -1.0f;
		drawParameter.ZFar = 1.0f;
		drawParameter.ViewProjectionMatrix = renderer->GetCameraProjectionMatrix();
		manager->Draw(drawParameter);

		// Finish to rendering effects
		// エフェクトの描画終了処理を行う。
		renderer->EndRendering();

		// Execute functions about OpenGL
		// OpenGLの処理
		device.PresentDevice();

		time++;
	}

	// Dispose the manager
	// マネージャーの破棄
	manager.Reset();

	// Dispose the renderer
	// レンダラーの破棄
	renderer.Reset();

	device.Terminate();

	return 0;
}
