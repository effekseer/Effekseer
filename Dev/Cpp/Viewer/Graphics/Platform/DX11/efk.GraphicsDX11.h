
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RendererImplemented.h>

#include "../../efk.Graphics.h"

namespace efk
{
class RenderTextureDX11 : public RenderTexture
{
private:
	Graphics* graphics = nullptr;

	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* textureSRV = nullptr;
	ID3D11RenderTargetView* textureRTV = nullptr;
	DXGI_FORMAT dxgiFormat_ = DXGI_FORMAT_UNKNOWN;

public:
	RenderTextureDX11(Graphics* graphics);
	virtual ~RenderTextureDX11();
	bool Initialize(Effekseer::Tool::Vector2DI size, TextureFormat format, uint32_t multisample = 1);

	ID3D11Texture2D* GetTexture() const
	{
		return texture;
	}

	ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return textureRTV;
	}

	ID3D11ShaderResourceView* GetShaderResourceView() const
	{
		return textureSRV;
	}

	uint64_t GetViewID() override
	{
		return (uint64_t)textureSRV;
	}

	DXGI_FORMAT GetDXGIFormat() const
	{
		return dxgiFormat_;
	}
};

class DepthTextureDX11 : public DepthTexture
{
private:
	Graphics* graphics = nullptr;
	int32_t width = 0;
	int32_t height = 0;

	ID3D11Texture2D* depthBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11ShaderResourceView* depthSRV = nullptr;

public:
	DepthTextureDX11(Graphics* graphics);
	virtual ~DepthTextureDX11();
	bool Initialize(int32_t width, int32_t height, uint32_t multisample = 1);

	ID3D11DepthStencilView* GetDepthStencilView() const
	{
		return depthStencilView;
	}

	ID3D11ShaderResourceView* GetShaderResourceView() const
	{
		return depthSRV;
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

	ID3D11Texture2D* defaultRenderTarget = nullptr;
	ID3D11Texture2D* defaultDepthStencil = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;

	//ID3D11Texture2D* backTexture = nullptr;
	//ID3D11ShaderResourceView* backTextureSRV = nullptr;

	ID3D11RenderTargetView* currentRenderTargetView = nullptr;
	ID3D11DepthStencilView* currentDepthStencilView = nullptr;

	RenderTexture* backupRenderTarget = nullptr;
	DepthTexture* backupDepthStencil = nullptr;

	/*
	LPDIRECT3D9			d3d = nullptr;
	LPDIRECT3DDEVICE9	d3d_device = nullptr;

	IDirect3DSurface9*	renderDefaultTarget = nullptr;
	IDirect3DSurface9*	renderDefaultDepth = nullptr;

	IDirect3DSurface9*	backTarget = nullptr;
	IDirect3DTexture9*	backTargetTexture = nullptr;
	*/

	ID3D11RasterizerState* rasterizerState = nullptr;
	ID3D11RasterizerState* savedRasterizerState = nullptr;

public:
	GraphicsDX11();
	virtual ~GraphicsDX11();

	bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode) override;
	
	void CopyTo(RenderTexture* src, RenderTexture* dst) override;

	//void CopyToBackground() override;

	void Resize(int32_t width, int32_t height) override;

	bool Present() override;

	void BeginScene() override;

	void EndScene() override;

	void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture) override;

//	void BeginRecord(int32_t width, int32_t height) override;
//
//	void EndRecord(std::vector<Effekseer::Color>& pixels) override;

	void SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels) override;

	void Clear(Effekseer::Color color) override;

	void ResolveRenderTarget(RenderTexture* src, RenderTexture* dest) override;

	bool CheckFormatSupport(TextureFormat format, TextureFeatureType feature) override;

	int GetMultisampleLevel(TextureFormat format) override;

	void ResetDevice() override;

	//void* GetBack() override;

	ID3D11Device* GetDevice() const;

	ID3D11DeviceContext* GetContext() const;

	DeviceType GetDeviceType() const override
	{
		return DeviceType::DirectX11;
	}
};
} // namespace efk
