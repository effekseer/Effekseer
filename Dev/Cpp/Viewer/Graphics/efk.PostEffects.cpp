#include "efk.PostEffects.h"

#ifdef _WIN32
//#include "Platform/DX9/efk.PostEffectsDX9.h"
#include "Platform/DX11/efk.PostEffectsDX11.h"
#endif

#include "Platform/GL/efk.PostEffectsGL.h"

namespace efk
{
	BloomEffect* PostEffect::CreateBloom(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX9)
		{
			//return new BloomEffectDX9(graphics);
		}

		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new BloomEffectDX11(graphics);
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return nullptr;
			return new BloomEffectGL(graphics);
		}

		return nullptr;
	}

	TonemapEffect* PostEffect::CreateTonemap(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX9)
		{
			//return new TonemapEffectDX9(graphics);
		}

		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new TonemapEffectDX11(graphics);
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			//return new TonemapEffectGL(graphics);
		}

		return nullptr;
	}
}