#pragma once

#include <XAudio2.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "../Utils/Window.h"

class DeviceDX11
{
private:
	std::shared_ptr<Utils::Window> window;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDXGIDevice1> dxgiDevice;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<IDXGIFactory> dxgiFactory;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11Texture2D> backBuffer;
	ComPtr<ID3D11Texture2D> depthBuffer;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

	ComPtr<ID3D11Device> dx11Device;
	ComPtr<ID3D11DeviceContext> dx11Context;

	ComPtr<IXAudio2> xa2Device;
	IXAudio2MasteringVoice* xa2MasterVoice = nullptr;

public:
	DeviceDX11() = default;
	~DeviceDX11() { Terminate(); }

	Utils::Vec2I GetWindowSize() const { return window->GetWindowSize(); }
	ID3D11Device* GetID3D11Device() { return dx11Device.Get(); }
	ID3D11DeviceContext* GetID3D11DeviceContext() { return dx11Context.Get(); }
	IXAudio2* GetIXAudio2() { return xa2Device.Get(); }

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	void PresentDevice();
	bool NewFrame() { return window->OnNewFrame(); }
};
