#include "RenderingWindowGL.h"
#include <EffekseerRendererGL.h>

RenderingWindowGL::RenderingWindowGL(std::array<int32_t, 2> windowSize, const char* title)
	: RenderingWindow(true, windowSize, title)
{
}

Effekseer::Backend::GraphicsDeviceRef RenderingWindowGL::GenerateGraphicsDevice()
{
	return EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
}