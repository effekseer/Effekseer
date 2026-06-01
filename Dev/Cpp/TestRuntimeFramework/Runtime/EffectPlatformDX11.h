#pragma once

#include "../../EffekseerRendererDX11/EffekseerRendererDX11.h"
#include "EffectPlatformGLFW.h"
#include <EffekseerToolRuntime/GroundRendering.h>

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
	ID3D11Texture2D* groundDepthTexture_ = nullptr;
	ID3D11RenderTargetView* groundDepthRenderTargetView_ = nullptr;
	ID3D11ShaderResourceView* groundDepthShaderResourceView_ = nullptr;
	ID3D11Texture2D* groundDepthBuffer_ = nullptr;
	ID3D11DepthStencilView* groundDepthStencilView_ = nullptr;
	ID3D11VertexShader* groundVertexShader_ = nullptr;
	ID3D11PixelShader* groundPixelShader_ = nullptr;
	ID3D11PixelShader* groundDepthPixelShader_ = nullptr;
	ID3D11InputLayout* groundInputLayout_ = nullptr;
	ID3D11Buffer* groundVertexBuffer_ = nullptr;
	ID3D11Buffer* groundIndexBuffer_ = nullptr;
	ID3D11RasterizerState* groundRasterizerState_ = nullptr;
	ID3D11DepthStencilState* groundDepthStencilState_ = nullptr;
	ID3D11BlendState* groundBlendState_ = nullptr;
	Effekseer::Backend::TextureRef groundDepthTextureForEffekseer_ = nullptr;

	bool isDefferedContextMode_ = true;
	bool usesGpuGroundDepth_ = false;

	void CreateCheckedTexture();
	bool CreateGroundResources();
	void ReleaseGroundResources();
	void UpdateGroundVertexBuffer(ID3D11DeviceContext* context);
	void DrawGround(ID3D11DeviceContext* context, Effekseer::ToolRuntime::GroundRenderPass pass);

protected:
	void UpdateBackgroundTexture() override;
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
	void ResetBackgroundPattern() override;
	void GenerateGroundDepth() override;
};
