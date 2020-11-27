#include "efk.PostEffects.h"

#ifdef _WIN32
#include "Platform/DX11/efk.PostEffectsDX11.h"
#endif

#include "Platform/GL/efk.PostEffectsGL.h"

namespace efk
{
BloomEffect* PostEffect::CreateBloom(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new BloomEffectDX11(graphics, renderer);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new BloomEffectGL(graphics, renderer);
	}

	return nullptr;
}

TonemapEffect* PostEffect::CreateTonemap(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new TonemapEffectDX11(graphics, renderer);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new TonemapEffectGL(graphics, renderer);
	}

	return nullptr;
}

LinearToSRGBEffect* PostEffect::CreateLinearToSRGB(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new LinearToSRGBEffectDX11(graphics, renderer);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new LinearToSRGBEffectGL(graphics, renderer);
	}

	return nullptr;
}
} // namespace efk