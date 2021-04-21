#pragma once

#include <array>
#include <stdint.h>

struct GLFWwindow;

class RenderingWindow
{
private:
	GLFWwindow* glfwWindow_ = nullptr;
	bool isOpenGLMode_ = false;

public:
	RenderingWindow(bool isOpenGLMode, std::array<int32_t, 2> windowSize, const char* title);
	virtual ~RenderingWindow();
	virtual void Present();
	virtual bool DoEvent();
	void* GetNativePtr(int32_t index);
};