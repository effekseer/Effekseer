
#include <stdio.h>

#include "../OpenGL/DeviceGLFW.h"
#include <Effekseer.h>
#include <EffekseerRendererGL.h>

int main(int argc, char** argv)
{
	DeviceGLFW device;
	device.Initialize("NetworkClient", Utils::Vec2I{1280, 720});

	// Effekseer's objects are managed with smart pointers. When the variable runs out, it will be disposed automatically.
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(8000);

	// Create a client for effect
	// クライアントの生成
	auto efkClient = Effekseer::Client::Create();

	// Start the client on port 60000
	// クライアントをポート60000で開始
	efkClient->Start("127.0.0.1", 60000);

	int32_t time = 0;

	while (device.NewFrame())
	{
		// Update the client
		// クライアントの更新を行う。
		// efkClient->Update();

		if (time % 120 == 0)
		{
			efkClient->Reload(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"Laser01_reload.efkefc", u"Laser01");
		}

		// Execute functions about Rendering
		// 描画の処理
		device.ClearScreen();

		// Execute functions about DirectX
		// DirectXの処理
		device.PresentDevice();

		time++;
	}

	// Dispose the client
	// クライアントの破棄
	efkClient.Reset();

	// Dispose the manager
	// マネージャーの破棄
	efkManager.Reset();

	return 0;
}
