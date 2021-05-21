
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RendererImplemented.h>
#include <EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h>

#include "../../efk.Graphics.h"

namespace efk
{
class RenderTextureDX11 : public RenderTexture
{
private:
	Graphics* graphics = nullptr;
	EffekseerRendererDX11::Backend::TextureRef texture_;

public:
	RenderTextureDX11(Graphics* graphics);
	virtual ~RenderTextureDX11();
	bool Initialize(Effekseer::Tool::Vector2DI size, Effekseer::Backend::TextureFormatType format, uint32_t multisample = 1);

	Effekseer::Backend::TextureRef GetAsBackend() override
	{
		return texture_;
	}

	ID3D11Texture2D* GetTexture() const
	{
		return texture_->GetTexture();
	}

	uint64_t GetViewID() override
	{
		return (uint64_t)texture_->GetSRV();
	}

	DXGI_FORMAT GetDXGIFormat() const
	{
		return EffekseerRendererDX11::Backend::GetTextureFormatType(texture_->GetFormat());
	}
};

class DepthTextureDX11 : public DepthTexture
{
private:
	Graphics* graphics = nullptr;
	EffekseerRendererDX11::Backend::TextureRef texture_;

public:
	DepthTextureDX11(Graphics* graphics);
	virtual ~DepthTextureDX11();
	bool Initialize(int32_t width, int32_t height, uint32_t multisample = 1);

	Effekseer::Backend::TextureRef GetAsBackend() override
	{
		return texture_;
	}

	ID3D11Texture2D* GetTexture() const
	{
		return texture_->GetTexture();
	}
};

class GraphicsDX11 : public Graphics
{
private:
	void* windowHandle = nullptr;
	int32_t windowWidth = 0;
	int32_t windowHeight = 0;
	bool isSRGBMode = false;

	ID3D11Device* device_ = nullptr;
	ID3D11Debug* d3dDebug = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGIDevice1* dxgiDevice = nullptr;
	IDXGIAdapter* adapter = nullptr;
	IDXGIFactory* dxgiFactory = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	//! default
	ID3D11Texture2D* defaultRenderTarget = nullptr;
	ID3D11Texture2D* defaultDepthStencil = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	std::array<ID3D11RenderTargetView*, 4> currentRenderTargetViews;
	ID3D11DepthStencilView* currentDepthStencilView = nullptr;

	RenderTexture* backupRenderTarget = nullptr;
	DepthTexture* backupDepthStencil = nullptr;

	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11RasterizerState* savedRasterizerState = nullptr;
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice_;
	D3D_FEATURE_LEVEL flevel_{};

public:
	GraphicsDX11();
	virtual ~GraphicsDX11();

	bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight) override;

	void CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst) override;

	void Resize(int32_t width, int32_t height) override;

	bool Present() override;

	void BeginScene() override;

	void EndScene() override;

	void SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture) override;

	void SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels) override;

	void Clear(Effekseer::Color color) override;

	void ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	bool CheckFormatSupport(Effekseer::Backend::TextureFormatType format, TextureFeatureType feature) override;

	int GetMultisampleLevel(Effekseer::Backend::TextureFormatType format) override;

	void ResetDevice() override;

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetContext() const;

	DeviceType GetDeviceType() const override
	{
		return DeviceType::DirectX11;
	}

	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> GetGraphicsDevice() override
	{
		return graphicsDevice_;
	}
};
} // namespace efk
