#include "efk.LineRenderer.h"

#include "Platform/DX9/efk.ImageRendererDX9.h"
#include "Platform/GL/efk.ImageRendererGL.h"

namespace efk
{
	ImageRenderer* ImageRenderer::Create(Graphics* graphics)
	{
		if (graphics->GetDeviceType() == DeviceType::DirectX9)
		{
			return new ImageRendererDX9(graphics->GetRenderer());
		}

		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new ImageRendererGL(graphics->GetRenderer());
		}

		return nullptr;
	}
}