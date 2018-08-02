
#include "efk.Graphics.h"

#ifdef _WIN32
#include "Platform/DX9/efk.GraphicsDX9.h"
#include "Platform/DX11/efk.GraphicsDX11.h"
#endif

#include "Platform/GL/efk.GraphicsGL.h"

namespace efk
{

RenderTexture* RenderTexture::Create(Graphics* graphics)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX9)
	{
		return new RenderTextureDX9(graphics);
	}
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new RenderTextureDX11(graphics);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new RenderTextureGL(graphics);
	}

	return nullptr;
}

DepthTexture* DepthTexture::Create(Graphics* graphics)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX9)
	{
		return new DepthTextureDX9(graphics);
	}
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new DepthTextureDX11(graphics);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new DepthTextureGL(graphics);
	}

	return nullptr;
}

}