
#pragma once

#include <Effekseer.h>

#include <functional>
#include <vector>

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../efk.Base.h"

namespace efk
{
	class Graphics;

	enum class TextureFormat {
		RGBA8U,
		RGBA16F,
		R16F,
	};

	class RenderTexture
	{
	public:
		RenderTexture() = default;
		virtual ~RenderTexture() = default;

		virtual bool Initialize(int32_t width, int32_t height, TextureFormat format, uint32_t multisample = 1) = 0;

		virtual int32_t GetWidth() = 0;
		virtual int32_t GetHeight() = 0;

		virtual uint64_t GetViewID() = 0;

		static RenderTexture* Create(Graphics* graphics);
	};

	class DepthTexture
	{
	public:
		DepthTexture() = default;
		virtual ~DepthTexture() = default;

		virtual bool Initialize(int32_t width, int32_t height, uint32_t multisample = 1) = 0;

		static DepthTexture* Create(Graphics* graphics);
	};

	class Graphics
	{
	protected:
		RenderTexture* currentRenderTexture = nullptr;
		DepthTexture* currentDepthTexture = nullptr;

	public:
		Graphics() {}
		virtual ~Graphics() {}

		virtual bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount) = 0;

		virtual void CopyToBackground() = 0;

		virtual void Resize(int32_t width, int32_t height) = 0;

		virtual bool Present() = 0;

		virtual void BeginScene() = 0;

		virtual void EndScene() = 0;

		virtual void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture) = 0;

		virtual void BeginRecord(int32_t width, int32_t height) = 0;

		virtual void EndRecord(std::vector<Effekseer::Color>& pixels) = 0;

		virtual void Clear(Effekseer::Color color) = 0;

		virtual void ResetDevice() = 0;

		virtual void* GetBack() = 0;

		virtual EffekseerRenderer::Renderer* GetRenderer() = 0;

		virtual DeviceType GetDeviceType() const = 0;

		virtual RenderTexture* GetRenderTexture() const { return currentRenderTexture; }
		virtual DepthTexture* GetDepthTexture() const { return currentDepthTexture; }

		virtual void ResolveRenderTarget(RenderTexture* src, RenderTexture* dest) {}

		/**
		Called when device is losted.
		*/
		std::function<void()>	LostedDevice;

		/**
		Called when device is resetted.
		*/
		std::function<void()>	ResettedDevice;

		/**
		Called when device is presented.
		*/
		std::function<void()>	Presented;
	};
}