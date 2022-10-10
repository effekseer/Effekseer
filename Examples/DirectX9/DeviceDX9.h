#pragma once

#include "../Utils/Input.h"
#include "../Utils/Window.h"
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>
#include <XAudio2.h>
#include <d3d9.h>
#include <functional>
#include <wrl/client.h>

class DeviceDX9
{
private:
	std::shared_ptr<Utils::Window> window;

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IDirect3D9> d3d9;
	ComPtr<IDirect3DDevice9> d3d9Device;

	ComPtr<IXAudio2> xa2Device;
	IXAudio2MasteringVoice* xa2MasterVoice = nullptr;

	::EffekseerRendererDX9::RendererRef efkRenderer;
	::EffekseerSound::SoundRef efkSound;

public:
	std::function<void()> onLostDevice;
	std::function<void()> onResetDevice;

public:
	DeviceDX9() = default;
	~DeviceDX9()
	{
		Terminate();
	}

	Utils::Vec2I GetWindowSize() const
	{
		return window->GetWindowSize();
	}
	IDirect3D9* GetIDirect3D9()
	{
		return d3d9.Get();
	}
	IDirect3DDevice9* GetIDirect3DDevice9()
	{
		return d3d9Device.Get();
	}
	IXAudio2* GetIXAudio2()
	{
		return xa2Device.Get();
	}

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	void PresentDevice();
	bool NewFrame();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);
	::EffekseerRendererDX9::RendererRef GetEffekseerRenderer()
	{
		return efkRenderer;
	}
};
