#pragma once

#include <glfw/glfw3.h>
#include "../Utils/Window.h"

class DeviceGLFW
{
private:
	GLFWwindow* glfwWindow = nullptr;

public:
	DeviceGLFW() = default;
	~DeviceGLFW() { Terminate(); }

	Utils::Vec2I GetWindowSize() const;

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	bool NewFrame();
	void PresentDevice();
};
