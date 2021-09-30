#pragma once

#include "../../EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h"
#include "../Window/RenderingWindowGL.h"
#include <EffekseerRendererGL.h>

#ifdef _WIN32
#include "../../EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h"
#include "../Window/RenderingWindowDX11.h"
#include <EffekseerRendererDX11.h>
#endif

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

Effekseer::Backend::RenderPassRef GenerateRenderPass(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderingWindow* window);

Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowGL* window);

#ifdef _WIN32
Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice(RenderingWindowDX11* window);
#endif