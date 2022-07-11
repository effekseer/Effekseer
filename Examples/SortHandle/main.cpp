﻿
#include <stdio.h>
#include <string>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include "../OpenGL/DeviceGLFW.h"

int main(int argc, char** argv)
{
	DeviceGLFW device;
	device.Initialize("SortHandle", { 1280, 720 });

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	auto efkRenderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

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

	// Specify a position of view
	// 視点位置を確定
	auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	// Specify a projection matrix
	// 投影行列を設定
	efkRenderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, 
		(float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f));

	// Specify a camera matrix
	// カメラ行列を設定
	efkRenderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	// Load effects
	// エフェクトの読込
	Effekseer::EffectRef effects[3];
	effects[0] = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"square_r.efkefc");
	effects[1] = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"square_g.efkefc");
	effects[2] = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"square_b.efkefc");

	int32_t time = 0;
	bool reversed = false;

	Effekseer::Handle handles[3];

	while (device.NewFrame())
	{
		if (time % 120 == 0)
		{
			// Stop effects
			// エフェクトの停止
			for (size_t i = 0; i < 3; i++)
			{
				efkManager->StopEffect(handles[i]);
			}

			// Play effects
			// エフェクトの再生
			for (int i = 0; i < 3; i++)
			{
				handles[i] = efkManager->Play(effects[i], (i - 1) * 10.0f, 0, 0);
			}

			reversed = !reversed;
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

		// Execute functions about OpenGL
		// OpenGLの処理
		device.ClearScreen();

		// Begin to rendering effects
		// エフェクトの描画開始処理を行う。
		efkRenderer->BeginRendering();

		// Render effects individually
		// エフェクトを個別に描画を行う。
		if (reversed)
		{
			for (int i = 0; i < 3; i++)
			{
				efkManager->DrawHandle(handles[i]);
			}
		}
		else
		{
			for (int i = 3 - 1; i > -1; i--)
			{
				efkManager->DrawHandle(handles[i]);
			}
		}

		// Finish to rendering effects
		// エフェクトの描画終了処理を行う。
		efkRenderer->EndRendering();

		// Execute functions about OpenGL
		// OpenGLの処理
		device.PresentDevice();

		time++;
	}

	// Dispose the manager
	// マネージャーの破棄
	efkManager.Reset();

	// Dispose the renderer
	// レンダラーの破棄
	efkRenderer.Reset();

	device.Terminate();

	return 0;
}
