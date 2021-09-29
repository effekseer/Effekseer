#include "Helper.h"

Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindowGL* window)
{
	return nullptr;
}

#ifdef _WIN32
Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindowDX11* window)
{
	auto gd = static_cast<EffekseerRendererDX11::Backend::GraphicsDevice*>(graphicsDevice.Get());
	auto rt = gd->CreateTexture(nullptr, window->GetRenderTargetView(), nullptr);
	auto dt = gd->CreateTexture(nullptr, nullptr, window->GetDepthStencilView());

	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> rts;
	rts.resize(1);
	rts.at(0) = rt;

	auto rp = gd->CreateRenderPass(rts, dt);
	return rp;
}
#endif

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