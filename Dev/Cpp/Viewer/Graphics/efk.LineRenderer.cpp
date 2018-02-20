#include "efk.LineRenderer.h"

#include "Platform/DX9/efk.LineRendererDX9.h"
#include "Platform/GL/efk.LineRendererGL.h"

namespace efk
{
	LineRenderer* LineRenderer::Create(Graphics* graphics)
	{
		if (graphics->GetDeviceType() == DeviceType::DirectX9)
		{
			return new LineRendererDX9(graphics->GetRenderer());
		}

		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new LineRendererGL(graphics->GetRenderer());
		}

		return nullptr;
	}
}