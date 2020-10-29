#pragma once

#include <array>
#include <stdint.h>

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

class RenderingWindow
{
private:
	GLFWwindow* glfwWindow_ = nullptr;
	bool isOpenGLMode_ = false;

public:
	RenderingWindow(bool isOpenGLMode, std::array<int32_t,2> windowSize, const char* title);
	virtual ~RenderingWindow();
	void Present();
	bool DoEvent();
	void* GetNativePtr(int32_t index);
};