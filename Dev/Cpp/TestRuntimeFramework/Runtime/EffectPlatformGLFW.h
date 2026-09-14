#pragma once

#include "EffectPlatform.h"

#if defined(WIN32) || defined(__APPLE__) || defined(__linux__)

#include <GLFW/glfw3.h>

#endif

class EffectPlatformGLFW : public EffectPlatform
{
private:
	GLFWwindow* glfwWindow_ = nullptr;

protected:
	void* GetNativePtr(int32_t index) override;
	void Present() override;
	bool DoEvent() override;

	void InitializeWindow() override;

public:
	EffectPlatformGLFW(bool isOpenGLMode);
	virtual ~EffectPlatformGLFW();
};
