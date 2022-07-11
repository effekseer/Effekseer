#include "DeviceGLFW.h"

Utils::Vec2I DeviceGLFW::GetWindowSize() const
{
	int width = 0, height = 0;
	glfwGetWindowSize(glfwWindow, &width, &height);
	return { width, height };
}

bool DeviceGLFW::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	// Initialize Window
	// ウインドウの初期化
	if (!glfwInit())
	{
		Terminate();
		return false;
	}

#if !_WIN32
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	glfwWindow = glfwCreateWindow(windowSize.X, windowSize.Y, windowTitle, nullptr, nullptr);

	if (glfwWindow == nullptr)
	{
		Terminate();
		return false;
	}

	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1);

	return true;
}

void DeviceGLFW::Terminate()
{
	if (glfwWindow != nullptr)
	{
		glfwDestroyWindow(glfwWindow);
		glfwWindow = nullptr;
		glfwTerminate();
	}
}

void DeviceGLFW::ClearScreen()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool DeviceGLFW::NewFrame()
{
	if (glfwWindowShouldClose(glfwWindow))
	{
		return false;
	}

	glfwPollEvents();

	return true;
}

void DeviceGLFW::PresentDevice()
{
	glfwSwapBuffers(glfwWindow);
}
