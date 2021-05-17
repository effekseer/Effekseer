
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
	R32F,
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
	Effekseer::Backend::TextureRef texture_;

public:
	RenderTexture() = default;
	virtual ~RenderTexture() = default;

	virtual bool Initialize(Effekseer::Tool::Vector2DI size, TextureFormat format, uint32_t multisample = 1) = 0;

	virtual uint64_t GetViewID() = 0;

	Effekseer::Backend::TextureRef GetAsBackend()
	{
		return texture_;
	}

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
public:
	Graphics()
	{
	}
	virtual ~Graphics()
	{
	}

	virtual bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight) = 0;

	virtual void CopyTo(RenderTexture* src, RenderTexture* dst) = 0;

	virtual void Resize(int32_t width, int32_t height) = 0;

	virtual bool Present() = 0;

	virtual void BeginScene() = 0;

	virtual void EndScene() = 0;

	virtual void SetRenderTarget(RenderTexture** renderTextures, int32_t renderTextureCount, DepthTexture* depthTexture) = 0;

	void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture)
	{
		if (renderTexture == nullptr)
		{
			SetRenderTarget(nullptr, 0, depthTexture);
		}
		else
		{
			SetRenderTarget(&renderTexture, 1, depthTexture);
		}
	}

	virtual void SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels) = 0;

	virtual void Clear(Effekseer::Color color) = 0;

	virtual void ResetDevice() = 0;

	//virtual void* GetBack() = 0;

	virtual DeviceType GetDeviceType() const = 0;

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

	std::function<void()> Presented;

	virtual Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> GetGraphicsDevice()
	{
		return nullptr;
	}
};
} // namespace efk