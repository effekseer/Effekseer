#pragma once

#include "RenderingWindow.h"
#include <d3d11.h>

class RenderingWindowDX11 : RenderingWindow
{
private:
	ID3D11Device* device_ = nullptr;
	ID3D11DeviceContext* context_ = nullptr;
	IDXGIDevice1* dxgiDevice_ = nullptr;
	IDXGIAdapter* adapter_ = nullptr;
	IDXGIFactory* dxgiFactory_ = nullptr;
	IDXGISwapChain* swapChain_ = nullptr;
	ID3D11Texture2D* backBuffer_ = nullptr;
	ID3D11Texture2D* depthBuffer_ = nullptr;
	ID3D11RenderTargetView* renderTargetView_ = nullptr;
	ID3D11DepthStencilView* depthStencilView_ = nullptr;

public:
	RenderingWindowDX11(std::array<int32_t, 2> windowSize, const char* title);
	~RenderingWindowDX11() override;
	void Present() override;
	bool DoEvent() override;

	ID3D11Device* GetDevice() const
	{
		return device_;
	}
	ID3D11DeviceContext* GetContext() const
	{
		return context_;
	}

	ID3D11Texture2D* GetBackBuffer() const
	{
		return backBuffer_;
	}

	ID3D11Texture2D* GetDepthBuffer() const
	{
		return depthBuffer_;
	}

	ID3D11RenderTargetView* GetRenderTargetView() const
	{
		return renderTargetView_;
	}
	ID3D11DepthStencilView* GetDepthStencilView() const
	{
		return depthStencilView_;
	}
};