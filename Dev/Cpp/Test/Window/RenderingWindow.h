#pragma once

#include <Effekseer.h>
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
	virtual Effekseer::Backend::GraphicsDeviceRef GenerateGraphicsDevice()
	{
		return nullptr;
	}
	void* GetNativePtr(int32_t index);
};