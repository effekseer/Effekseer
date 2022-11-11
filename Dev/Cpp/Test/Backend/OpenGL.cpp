#include "../TestHelper.h"
#include "Helper.h"

#include "../../../EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h"
#include <RenderingEnvironment/RenderingEnvironmentGL.h>

#include <iostream>

void Backend_OpenGL()
{
	std::shared_ptr<RenderingEnvironment> window = std::make_shared<RenderingEnvironmentGL>(std::array<int, 2>({1280, 720}), "Backend.OpenGL");

	auto graphicsDevice = window->GetGraphicsDevice();
	auto graphicsDeviceGL = graphicsDevice.DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	std::cout << "MaxVaryingVectors : " << graphicsDeviceGL->GetProperty(EffekseerRendererGL::Backend::DevicePropertyType::MaxVaryingVectors) << std::endl;
	std::cout << "MaxVertexUniformVectors : " << graphicsDeviceGL->GetProperty(EffekseerRendererGL::Backend::DevicePropertyType::MaxVertexUniformVectors) << std::endl;
	std::cout << "MaxFragmentUniformVectors : " << graphicsDeviceGL->GetProperty(EffekseerRendererGL::Backend::DevicePropertyType::MaxFragmentUniformVectors) << std::endl;
	std::cout << "MaxVertexTextureImageUnits : " << graphicsDeviceGL->GetProperty(EffekseerRendererGL::Backend::DevicePropertyType::MaxVertexTextureImageUnits) << std::endl;
	std::cout << "MaxTextureImageUnits : " << graphicsDeviceGL->GetProperty(EffekseerRendererGL::Backend::DevicePropertyType::MaxTextureImageUnits) << std::endl;
}

#if !defined(__FROM_CI__)
TestRegister Test_Backend_OpenGL("Backend.OpenGL", []() -> void { Backend_OpenGL(); });

#endif