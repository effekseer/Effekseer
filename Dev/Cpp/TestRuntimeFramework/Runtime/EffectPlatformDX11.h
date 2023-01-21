#pragma once

#include "../../EffekseerRendererDX11/EffekseerRendererDX11.h"
#include "EffectPlatformGLFW.h"

class EffectPlatformDX11 final : public EffectPlatformGLFW
{
private:
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;
	ID3D11DeviceContext* defferedContext_ = nullptr;
	IDXGIDevice1* dxgiDevice_ = nullptr;
	IDXGIAdapter* adapter_ = nullptr;
	IDXGIFactory* dxgiFactory_ = nullptr;
	IDXGISwapChain* swapChain_ = nullptr;
	ID3D11Texture2D* backBuffer_ = nullptr;
	ID3D11Texture2D* depthBuffer_ = nullptr;
	ID3D11RenderTargetView* renderTargetView_ = nullptr;
	ID3D11DepthStencilView* depthStencilView_ = nullptr;
	ID3D11Texture2D* checkedTexture_ = nullptr;

	bool isDefferedContextMode_ = true;

	void CreateCheckedTexture();

protected:
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformDX11(bool isDefferedContextMode = false)
		: EffectPlatformGLFW(false)
		, isDefferedContextMode_(isDefferedContextMode)
	{
	}

	~EffectPlatformDX11();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	void Present() override;
	bool TakeScreenshot(const char* path) override;
};