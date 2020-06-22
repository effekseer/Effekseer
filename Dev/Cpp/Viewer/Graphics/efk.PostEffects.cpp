#include "efk.PostEffects.h"

#ifdef _WIN32
#include "Platform/DX11/efk.PostEffectsDX11.h"
#endif

#include "Platform/GL/efk.PostEffectsGL.h"

namespace efk
{
	BloomEffect* PostEffect::CreateBloom(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new BloomEffectDX11(graphics);
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new BloomEffectGL(graphics);
		}

		return nullptr;
	}

	TonemapEffect* PostEffect::CreateTonemap(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new TonemapEffectDX11(graphics);
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new TonemapEffectGL(graphics);
		}

		return nullptr;
	}

	LinearToSRGBEffect* PostEffect::CreateLinearToSRGB(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new LinearToSRGBEffectDX11(graphics);
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new LinearToSRGBEffectGL(graphics);
		}

		return nullptr;
	}
}