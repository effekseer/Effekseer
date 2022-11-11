#pragma once

#include "EffectPlatform.h"

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