
#include "Window.h"

void* Window::GetNativePtr(int32_t index)
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

Window::Window(bool isOpenGLMode, std::array<int32_t, 2> windowSize, const char* title)
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

Window::~Window()
{
	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}
}

void Window::Present()
{
	if (isOpenGLMode_)
	{
		glfwSwapBuffers(glfwWindow_);
	}
}

bool Window::DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow_) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	return true;
}
