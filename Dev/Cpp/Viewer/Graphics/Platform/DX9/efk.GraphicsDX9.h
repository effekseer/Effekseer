
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>

#include "../../efk.Graphics.h"

namespace efk
{
	class RenderTextureDX9
		: public RenderTexture
	{
	private:
		Graphics* graphics = nullptr;
		int32_t width = 0;
		int32_t height = 0;

		IDirect3DSurface9*	renderTarget = nullptr;
		IDirect3DTexture9*	renderTargetTexture = nullptr;

	public:
		RenderTextureDX9(Graphics* graphics);
		virtual ~RenderTextureDX9();
		bool Initialize(int32_t width, int32_t height);

		int32_t GetWidth() { return width; }
		int32_t GetHeight() { return height; }

		IDirect3DSurface9* GetSurface() const { return renderTarget; }

		uint64_t GetViewID() override { return (uint64_t)renderTargetTexture; }
	};

	class DepthTextureDX9
		: public DepthTexture
	{
	private:
		Graphics* graphics = nullptr;
		int32_t width = 0;
		int32_t height = 0;
		IDirect3DSurface9*	depthTexture = nullptr;
	public:
		DepthTextureDX9(Graphics* graphics);
		virtual ~DepthTextureDX9();
		bool Initialize(int32_t width, int32_t height);

		IDirect3DSurface9* GetSurface() const { return depthTexture; }
	};


	class GraphicsDX9
		: public Graphics
	{
	private:
		void*	windowHandle = nullptr;
		int32_t	windowWidth = 0;
		int32_t	windowHeight = 0;

		LPDIRECT3D9			d3d = nullptr;
		LPDIRECT3DDEVICE9	d3d_device = nullptr;
		bool				isSRGBMode = false;

		IDirect3DSurface9*	renderDefaultTarget = nullptr;
		IDirect3DSurface9*	renderDefaultDepth = nullptr;

		IDirect3DSurface9*	recordingTarget = nullptr;
		IDirect3DTexture9*	recordingTargetTexture = nullptr;
		IDirect3DSurface9*	recordingDepth = nullptr;
		int32_t				recordingWidth = 0;
		int32_t				recordingHeight = 0;

		IDirect3DSurface9*	backTarget = nullptr;
		IDirect3DTexture9*	backTargetTexture = nullptr;

		EffekseerRendererDX9::Renderer*	renderer = nullptr;

	public:
		GraphicsDX9();
		virtual ~GraphicsDX9();

		bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount) override;

		void CopyToBackground() override;

		void Resize(int32_t width, int32_t height) override;

		bool Present() override;

		void BeginScene() override;

		void EndScene() override;

		void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture) override;

		void BeginRecord(int32_t width, int32_t height) override;

		void EndRecord(std::vector<Effekseer::Color>& pixels) override;

		void Clear(Effekseer::Color color) override;

		void ResetDevice() override;

		void* GetBack() override;

		EffekseerRenderer::Renderer* GetRenderer() override;

		DeviceType GetDeviceType() const override { return DeviceType::DirectX9; }
	};
}