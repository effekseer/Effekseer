#include "efk.LineRenderer.h"

#ifdef _WIN32
#include "Platform/DX11/efk.ImageRendererDX11.h"
#endif

#include "Platform/GL/efk.ImageRendererGL.h"

namespace efk
{
ImageRenderer* ImageRenderer::Create(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
{
#ifdef _WIN32
	if (graphics->GetDeviceType() == DeviceType::DirectX11)
	{
		return new ImageRendererDX11(renderer);
	}
#endif
	if (graphics->GetDeviceType() == DeviceType::OpenGL)
	{
		return new ImageRendererGL(renderer);
	}

	return nullptr;
}
} // namespace efk