#pragma once

#include "../../EffekseerRendererDX9/EffekseerRendererDX9.h"
#include "EffectPlatformGLFW.h"

class DistortingCallbackDX9 : public EffekseerRenderer::DistortingCallback
{
	::EffekseerRendererDX9::Renderer* renderer = nullptr;
	LPDIRECT3DDEVICE9 device = nullptr;
	LPDIRECT3DTEXTURE9 texture = nullptr;
	int32_t texWidth_ = 0;
	int32_t texHeight_ = 0;

public:
	DistortingCallbackDX9(::EffekseerRendererDX9::Renderer* renderer, LPDIRECT3DDEVICE9 device, int texWidth, int texHeight);

	virtual ~DistortingCallbackDX9();

	virtual bool OnDistorting() override;

	void Lost();

	void Reset();
};

class EffectPlatformDX9 final : public EffectPlatformGLFW
{
private:
	LPDIRECT3D9 d3d_ = nullptr;
	LPDIRECT3DDEVICE9 device_ = nullptr;
	bool vsync_ = false;
	bool fullscreen_ = false;
	IDirect3DSurface9* checkedSurface_ = nullptr;
	DistortingCallbackDX9* distorting_ = nullptr;

	void CreateCheckedSurface();

protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformDX9()
		: EffectPlatformGLFW(false)
	{
	}

	~EffectPlatformDX9();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	void Present() override;
	bool TakeScreenshot(const char* path) override;
	bool SetFullscreen(bool isFullscreen) override;

	void ResetDevice();
};