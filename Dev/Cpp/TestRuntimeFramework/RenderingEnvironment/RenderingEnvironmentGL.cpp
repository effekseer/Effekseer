#include "RenderingEnvironmentGL.h"
#include <EffekseerRendererGL.h>

RenderingEnvironmentGL::RenderingEnvironmentGL(std::array<int32_t, 2> windowSize, const char* title)
	: RenderingEnvironment(true, windowSize, title)
{
	graphicsDevice_ = EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
}
