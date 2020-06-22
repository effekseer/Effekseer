#include "efk.LineRenderer.h"

#ifdef _WIN32
#include "Platform/DX11/efk.LineRendererDX11.h"
#endif

#include "Platform/GL/efk.LineRendererGL.h"

namespace efk
{
	LineRenderer* LineRenderer::Create(Graphics* graphics)
	{
#ifdef _WIN32
		if (graphics->GetDeviceType() == DeviceType::DirectX11)
		{
			return new LineRendererDX11(graphics->GetRenderer());
		}
#endif
		if (graphics->GetDeviceType() == DeviceType::OpenGL)
		{
			return new LineRendererGL(graphics->GetRenderer());
		}

		return nullptr;
	}
}