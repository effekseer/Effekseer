#include "efk.LineRenderer.h"

#ifdef _WIN32
#include "Platform/DX9/efk.ImageRendererDX9.h"
#include "Platform/DX11/efk.ImageRendererDX11.h"
#endif

#include "Platform/GL/efk.ImageRendererGL.h"

namespace efk
{
	ImageRenderer* ImageRenderer::Create(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX9)
		{
			return new ImageRendererDX9(graphics->GetRenderer());
		}

		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new ImageRendererDX11(graphics->GetRenderer());
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new ImageRendererGL(graphics->GetRenderer());
		}

		return nullptr;
	}
}