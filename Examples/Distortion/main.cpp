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

#if defined(DEVICE_OPENGL)
class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
	DeviceGLFW& device;
	GLuint backgroundTexture = 0;

public:
	DistortingCallback(DeviceGLFW& device)
		: device(device)
	{
		glGenTextures(1, &backgroundTexture);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenSize.X, screenSize.Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual ~DistortingCallback()
	{
		glDeleteTextures(1, &backgroundTexture);
	}

	bool OnDistorting()
	{
		if (CopyRenderTargetToBackground())
		{
			device.GetEffekseerRenderer()->SetBackground(backgroundTexture);
			return true;
		}
		return false;
	}

private:
	bool CopyRenderTargetToBackground()
	{
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, screenSize.X, screenSize.Y);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}
};
#elif defined(DEVICE_DX9)
class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
	DeviceDX9& device;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<IDirect3DSurface9> backgroundSurface;
	ComPtr<IDirect3DTexture9> backgroundTexture;

public:
	DistortingCallback(DeviceDX9& device)
		: device(device)
	{
		device.GetIDirect3DDevice9()->CreateTexture(screenSize.X, screenSize.Y, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, backgroundTexture.GetAddressOf(), nullptr);

		backgroundTexture->GetSurfaceLevel(0, backgroundSurface.GetAddressOf());
	}

	virtual ~DistortingCallback()
	{
	}

	bool OnDistorting()
	{
		if (CopyRenderTargetToBackground())
		{
			device.GetEffekseerRenderer()->SetBackground(backgroundTexture.Get());
			return true;
		}
		return false;
	}

private:
	bool CopyRenderTargetToBackground()
	{
		auto d3d9Device = device.GetIDirect3DDevice9();

		HRESULT hr = S_OK;
		ComPtr<IDirect3DSurface9> tempRender;
		ComPtr<IDirect3DSurface9> tempDepth;

		hr = d3d9Device->GetRenderTarget(0, tempRender.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		hr = d3d9Device->GetDepthStencilSurface(tempDepth.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		d3d9Device->SetRenderTarget(0, backgroundSurface.Get());
		d3d9Device->SetDepthStencilSurface(nullptr);

		D3DSURFACE_DESC desc{};
		tempRender->GetDesc(&desc);

		d3d9Device->StretchRect(tempRender.Get(), nullptr, backgroundSurface.Get(), nullptr, D3DTEXF_POINT);

		d3d9Device->SetRenderTarget(0, tempRender.Get());
		d3d9Device->SetDepthStencilSurface(tempDepth.Get());

		return true;
	}
};
backBuffer
#elif defined(DEVICE_DX11)
class DistortingCallback : public EffekseerRenderer::DistortingCallback
{
	DeviceDX11& device;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Texture2D> backgroundTexture;
	ComPtr<ID3D11ShaderResourceView> backgroundTextureView;

public:
	DistortingCallback(DeviceDX11& device)
		: device(device)
	{
		auto dx11Device = device.GetID3D11Device();
		assert(dx11Device != nullptr);

		HRESULT hr = S_OK;

		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = screenSize.X;
		texture2dDesc.Height = screenSize.Y;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		hr = dx11Device->CreateTexture2D(&texture2dDesc, nullptr, backgroundTexture.GetAddressOf());
		assert(hr == S_OK);

		D3D11_SHADER_RESOURCE_VIEW_DESC texviewDesc{};
		texviewDesc.Format = texture2dDesc.Format;
		texviewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		texviewDesc.Texture2D.MostDetailedMip = 0;
		texviewDesc.Texture2D.MipLevels = 1;
		hr = dx11Device->CreateShaderResourceView(backgroundTexture.Get(), &texviewDesc, backgroundTextureView.GetAddressOf());
		assert(hr == S_OK);
	}

	virtual ~DistortingCallback()
	{
	}

	bool OnDistorting()
	{
		if (CopyRenderTargetToBackground())
		{
			device.GetEffekseerRenderer()->SetBackground(backgroundTextureView.Get());
			return true;
		}
		return false;
	}

private:
	bool CopyRenderTargetToBackground()
	{
		auto dx11Context = device.GetID3D11DeviceContext();
		assert(dx11Context != nullptr);

		dx11Context->CopyResource(backgroundTexture.Get(), device.GetBackBuffer());

		return true;
	}
};
#endif

int main(int argc, char** argv)
{
#if defined(DEVICE_OPENGL)
	DeviceGLFW device;
	device.Initialize("Distortion (OpenGL)", screenSize);
#elif defined(DEVICE_DX9)
	DeviceDX9 device;
	device.Initialize("Distortion (DirectX9)", screenSize);
#elif defined(DEVICE_DX11)
	DeviceDX11 device;
	device.Initialize("Distortion (DirectX11)", screenSize);
#endif

	// Create a manager of effects
	// エフェクトのマネージャーの作成
	auto efkManager = ::Effekseer::Manager::Create(2000);

	// Setup effekseer modules
	// Effekseerのモジュールをセットアップする
	device.SetupEffekseerModules(efkManager);
	auto efkRenderer = device.GetEffekseerRenderer();

	// Specify a distortion function
	// 歪み機能を設定
	// If you'd like to distort background and particles by rendering,
	// it need to specify a DistortingCallback. (It is a bit heavy)
	// もし、描画するごとに背景とパーティクルを歪ませたい場合、
	// DistortingCallbackを設定する必要がある (やや重い)

	// efkRenderer->SetDistortingCallback(new DistortingCallback());

	// Specify a position of view
	// 視点位置を確定
	auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f) * 0.25;

	// Specify a projection matrix
	// 投影行列を設定
	::Effekseer::Matrix44 projectionMatrix;
	projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14f, (float)device.GetWindowSize().X / (float)device.GetWindowSize().Y, 1.0f, 500.0f);

	// Specify a camera matrix
	// カメラ行列を設定
	::Effekseer::Matrix44 cameraMatrix;
	cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

	auto effect = Effekseer::Effect::Create(efkManager, EFK_EXAMPLE_ASSETS_DIR_U16 u"distortion.efk");

	// Play an effect
	// エフェクトの再生
	auto handle = efkManager->Play(effect, 0, 0, 0);

	int32_t time = 0;

	while (device.NewFrame())
	{
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

		{
			// Begin to rendering effects
			// エフェクトの描画開始処理を行う。
			efkRenderer->BeginRendering();

			// Render rear effects
			// 背面のエフェクトの描画を行う。
			Effekseer::Manager::DrawParameter drawParameter;
			drawParameter.ZNear = 0.0f;
			drawParameter.ZFar = 1.0f;
			drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
			efkManager->DrawBack(drawParameter);

			// Finish to rendering effects
			// エフェクトの描画終了処理を行う。
			efkRenderer->EndRendering();
		}

		// Distort background and rear effects.
		// 背景と背面のエフェクトを歪ませる。
		DistortingCallback distoring(device);
		distoring.OnDistorting();

		{
			// Begin to rendering effects
			// エフェクトの描画開始処理を行う。
			efkRenderer->BeginRendering();

			// Render front effects
			// 前面のエフェクトの描画を行う。
			Effekseer::Manager::DrawParameter drawParameter;
			drawParameter.ZNear = 0.0f;
			drawParameter.ZFar = 1.0f;
			drawParameter.ViewProjectionMatrix = efkRenderer->GetCameraProjectionMatrix();
			efkManager->DrawFront(drawParameter);

			// Finish to rendering effects
			// エフェクトの描画終了処理を行う。
			efkRenderer->EndRendering();
		}

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
