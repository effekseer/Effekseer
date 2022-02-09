
#pragma once

#include <Effekseer.h>

#include <functional>
#include <vector>

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../Math/Vector2I.h"
#include "../Parameters.h"

namespace efk
{
class Graphics;

enum class TextureFeatureType
{
	Texture2D,
	MultisampledTexture2DRenderTarget,
	MultisampledTexture2DResolve,
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

	virtual void CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst) = 0;

	virtual void Resize(int32_t width, int32_t height) = 0;

	virtual bool Present() = 0;

	virtual void SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture) = 0;

	virtual void SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels) = 0;

	virtual void Clear(Effekseer::Color color) = 0;

	virtual void ResetDevice() = 0;

	virtual Effekseer::Tool::DeviceType GetDeviceType() const = 0;

	virtual void ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
	{
	}

	virtual bool CheckFormatSupport(Effekseer::Backend::TextureFormatType format, TextureFeatureType feature)
	{
		return true;
	}

	virtual int GetMultisampleLevel(Effekseer::Backend::TextureFormatType format)
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