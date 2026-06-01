#pragma once

#include "../../EffekseerRendererDX9/EffekseerRendererDX9.h"
#include "EffectPlatformGLFW.h"
#include <EffekseerToolRuntime/GroundRendering.h>

struct IDirect3DVertexDeclaration9;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;

class DistortingCallbackDX9 : public EffekseerRenderer::DistortingCallback
{
	LPDIRECT3DDEVICE9 device = nullptr;
	LPDIRECT3DTEXTURE9 texture = nullptr;
	int32_t texWidth_ = 0;
	int32_t texHeight_ = 0;

public:
	DistortingCallbackDX9(::EffekseerRendererDX9::RendererRef renderer, LPDIRECT3DDEVICE9 device, int texWidth, int texHeight);

	virtual ~DistortingCallbackDX9();

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;

	void Lost();

	void ChangeDevice(LPDIRECT3DDEVICE9 device);

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
	LPDIRECT3DTEXTURE9 groundDepthTexture_ = nullptr;
	IDirect3DSurface9* groundDepthSurface_ = nullptr;
	IDirect3DSurface9* groundDepthStencilSurface_ = nullptr;
	IDirect3DVertexDeclaration9* groundVertexDeclaration_ = nullptr;
	IDirect3DVertexShader9* groundVertexShader_ = nullptr;
	IDirect3DPixelShader9* groundPixelShader_ = nullptr;
	IDirect3DPixelShader9* groundDepthPixelShader_ = nullptr;
	Effekseer::Backend::TextureRef groundDepthTextureForEffekseer_ = nullptr;
	bool usesGpuGroundDepth_ = false;

	void CreateCheckedSurface();
	bool CreateGroundResources();
	void ReleaseGroundResources();
	void DrawGround(Effekseer::ToolRuntime::GroundRenderPass pass);

protected:
	void UpdateBackgroundTexture() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;

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
	void ResetBackgroundPattern() override;
	void GenerateGroundDepth() override;

	void ResetDevice();
};
