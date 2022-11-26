#if !defined(__APPLE__)
#define GLEW_STATIC
typedef char GLchar;
#include <GL/glew.h>
#endif

#include "EffectPlatformGLFW.h"

#include <iostream>

#if !defined(__APPLE__)
void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type != GL_DEBUG_TYPE_PORTABILITY && type != GL_DEBUG_TYPE_OTHER)
	{
		std::cout << message << std::endl;
	}
}
#endif

void* EffectPlatformGLFW::GetNativePtr(int32_t index)
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

EffectPlatformGLFW::EffectPlatformGLFW(bool isOpenGLMode)
{
	isOpenGLMode_ = isOpenGLMode;
}

EffectPlatformGLFW ::~EffectPlatformGLFW()
{
	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}
}

void EffectPlatformGLFW::Present()
{
	if (isOpenGLMode_)
	{
		glfwSwapBuffers(glfwWindow_);
	}
}

bool EffectPlatformGLFW::DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow_) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

void EffectPlatformGLFW::InitializeWindow()
{
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

	if (isOpenGLMode_)
	{
#if !defined(__APPLE__)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

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

	glfwWindow_ = glfwCreateWindow(initParam_.WindowSize[0], initParam_.WindowSize[1], "Example glfw", nullptr, nullptr);

	if (glfwWindow_ == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	if (isOpenGLMode_)
	{
		glfwMakeContextCurrent(glfwWindow_);

#if !defined(__APPLE__)
		glewInit();

		GLint flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, NULL);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		}
#endif
	}
}
