
#pragma once

#include <Effekseer.h>

#include <functional>
#include <vector>

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../Math/Vector2DI.h"
#include "../efk.Base.h"

namespace Effekseer
{
namespace Tool
{

class RenderPass;
class CommandList;

} // namespace Tool
} // namespace Effekseer

namespace efk
{
class Graphics;

enum class TextureFormat
{
	RGBA8U,
	RGBA16F,
	R16F,
};

enum class TextureFeatureType
{
	Texture2D,
	MultisampledTexture2DRenderTarget,
	MultisampledTexture2DResolve,
};

class RenderTexture
{
protected:
	int32_t samplingCount_ = 1;
	Effekseer::Tool::Vector2DI size_;
	TextureFormat format_;

public:
	RenderTexture() = default;
	virtual ~RenderTexture() = default;

	virtual bool Initialize(Effekseer::Tool::Vector2DI size, TextureFormat format, uint32_t multisample = 1) = 0;

	virtual uint64_t GetViewID() = 0;

	Effekseer::Tool::Vector2DI GetSize() const
	{
		return size_;
	}

	int32_t GetSamplingCount() const
	{
		return samplingCount_;
	}

	TextureFormat GetFormat() const
	{
		return format_;
	}

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
	Graphics()
	{
	}
	virtual ~Graphics()
	{
	}

	virtual bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode) = 0;

	virtual void CopyTo(RenderTexture* src, RenderTexture* dst) = 0;

	//virtual void CopyToBackground() = 0;

	virtual void Resize(int32_t width, int32_t height) = 0;

	virtual bool Present() = 0;

	virtual void BeginScene() = 0;

	virtual void EndScene() = 0;

	virtual void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture) = 0;

	virtual void SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels) = 0;

	virtual void Clear(Effekseer::Color color) = 0;

	virtual void ResetDevice() = 0;

	//virtual void* GetBack() = 0;

	virtual DeviceType GetDeviceType() const = 0;

	virtual RenderTexture* GetRenderTexture() const
	{
		return currentRenderTexture;
	}
	virtual DepthTexture* GetDepthTexture() const
	{
		return currentDepthTexture;
	}

	virtual void ResolveRenderTarget(RenderTexture* src, RenderTexture* dest)
	{
	}

	virtual bool CheckFormatSupport(TextureFormat format, TextureFeatureType feature)
	{
		return true;
	}

	virtual int GetMultisampleLevel(TextureFormat format)
	{
		return 4;
	}

	virtual std::shared_ptr<Effekseer::Tool::RenderPass> CreateRenderPass(std::shared_ptr<efk::RenderTexture> colorTexture, std::shared_ptr<efk::DepthTexture> depthTexture)
	{
		return nullptr;
	}

	virtual std::shared_ptr<Effekseer::Tool::CommandList> CreateCommandList()
	{
		return nullptr;
	}

	/**
	Called when device is losted.
	*/
	//std::function<void()> LostedDevice;

	/**
	Called when device is resetted.
	*/
	//std::function<void()> ResettedDevice;

	/**
	Called when device is presented.
	*/
	std::function<void()> Presented;
};
} // namespace efk