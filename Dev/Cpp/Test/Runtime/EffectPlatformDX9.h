#pragma once

#include "../../EffekseerRendererDX9/EffekseerRendererDX9.h"
#include "EffectPlatformGLFW.h"

class EffectPlatformDX9 final : public EffectPlatformGLFW
{
private:
	LPDIRECT3D9 d3d_ = nullptr;
	LPDIRECT3DDEVICE9 device_ = nullptr;
	bool vsync_ = false;
	bool fullscreen_ = false;
	IDirect3DSurface9* checkedSurface_ = nullptr;

	void CreateCheckedSurface();

protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformDX9() : EffectPlatformGLFW(false) {}

	~EffectPlatformDX9();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	void Present() override;
	bool TakeScreenshot(const char* path) override;
	bool SetFullscreen(bool isFullscreen) override;

	void ResetDevice();
};