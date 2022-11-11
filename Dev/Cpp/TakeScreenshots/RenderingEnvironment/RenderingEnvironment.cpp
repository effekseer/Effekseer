
#include "RenderingEnvironment.h"

#if defined(WIN32) || defined(__APPLE__) || defined(__linux__)

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11 1
#undef Always
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef __linux__
#undef Always
#endif

#endif

void* RenderingEnvironment::GetNativePtr(int32_t index)
{
#ifdef _WIN32
	if (index == 0)
	{
		return glfwGetWin32Window(glfwWindow_);
	}

	return (HINSTANCE)GetModuleHandle(0);
#endif

#ifdef __APPLE__
	return glfwGetCocoaWindow(glfwWindow_);
#endif

#ifdef __linux__
	if (index == 0)
	{
		return glfwGetX11Display();
	}

	return reinterpret_cast<void*>(glfwGetX11Window(glfwWindow_));
#endif

	return nullptr;
}

RenderingEnvironment::RenderingEnvironment(bool isOpenGLMode, std::array<int32_t, 2> windowSize, const char* title)
{
	isOpenGLMode_ = isOpenGLMode;

	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

	if (isOpenGLMode_)
	{
#if !_WIN32
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	glfwWindow_ = glfwCreateWindow(windowSize[0], windowSize[1], title, nullptr, nullptr);

	if (glfwWindow_ == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	if (isOpenGLMode_)
	{
		glfwMakeContextCurrent(glfwWindow_);
	}
}

RenderingEnvironment::~RenderingEnvironment()
{
	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}
}

void RenderingEnvironment::Present()
{
	if (isOpenGLMode_)
	{
		glfwSwapBuffers(glfwWindow_);
	}
}

bool RenderingEnvironment::DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow_) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

Effekseer::Backend::ShaderRef RenderingEnvironment::CreateShader(std::map<std::string, ShaderContainer> shaders, Effekseer::Backend::UniformLayoutRef layout)
{
	const auto& shader = shaders[graphicsDevice_->GetDeviceName()];
	if (shader.VertexCodes.size() > 0)
	{
		Effekseer::CustomVector<Effekseer::StringView<char>> vsCodes;
		Effekseer::CustomVector<Effekseer::StringView<char>> psCodes;

		for (size_t i = 0; i < shader.VertexCodes.size(); i++)
		{
			vsCodes.emplace_back(shader.VertexCodes[i].c_str());
		}

		for (size_t i = 0; i < shader.PixelCodes.size(); i++)
		{
			psCodes.emplace_back(shader.PixelCodes[i].c_str());
		}

		return graphicsDevice_->CreateShaderFromCodes(vsCodes, psCodes, layout);
	}
	else if (shader.VertexData.size() > 0)
	{
		return graphicsDevice_->CreateShaderFromBinary(shader.VertexData.data(), shader.VertexData.size(), shader.PixelData.data(), shader.PixelData.size());
	}

	return nullptr;
}