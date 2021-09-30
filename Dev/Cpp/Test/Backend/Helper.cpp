#include "Helper.h"

Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindow* window)
{
#ifdef _WIN32
	if (graphicsDevice->GetDeviceName() == "DirectX11")
	{
		auto gd = static_cast<EffekseerRendererDX11::Backend::GraphicsDevice*>(graphicsDevice.Get());
		auto w = static_cast<RenderingWindowDX11*>(window);
		auto rt = gd->CreateTexture(nullptr, w->GetRenderTargetView(), nullptr);
		auto dt = gd->CreateTexture(nullptr, nullptr, w->GetDepthStencilView());

		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> rts;
		rts.resize(1);
		rts.at(0) = rt;

		auto rp = gd->CreateRenderPass(rts, dt);
		return rp;
	}
#endif

	return nullptr;
}

Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowGL* window)
{
	return EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
}

#ifdef _WIN32
Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowDX11* window)
{
	return EffekseerRendererDX11::CreateGraphicsDevice(window->GetDevice(), window->GetContext());
}
#endif