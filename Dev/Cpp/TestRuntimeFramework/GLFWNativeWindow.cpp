#include "GLFWNativeWindow.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA 1
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11 1
#endif

// Native platform headers are intentionally confined to this translation unit.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

void* GetGLFWNativeWindowPointer(GLFWwindow* window, int32_t index)
{
#ifdef _WIN32
	if (index == 0)
	{
		return glfwGetWin32Window(window);
	}

	return GetModuleHandle(nullptr);
#elif defined(__APPLE__)
	return glfwGetCocoaWindow(window);
#elif defined(__linux__)
	if (index == 0)
	{
		return glfwGetX11Display();
	}

	return reinterpret_cast<void*>(glfwGetX11Window(window));
#else
	return nullptr;
#endif
}
