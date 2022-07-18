#pragma once

#define USE_OPENAL 0

#include <glfw/glfw3.h>
#include <EffekseerRendererGL.h>
#if USE_OPENAL
#include <EffekseerRendererAL.h>
#endif
#include "../Utils/Window.h"

class DeviceGLFW
{
private:
	GLFWwindow* glfwWindow = nullptr;

	::EffekseerRendererGL::RendererRef efkRenderer;

#if USE_OPENAL
	::EffekseerSound::SoundRef efkSound;
#endif

public:
	DeviceGLFW() = default;
	~DeviceGLFW() { Terminate(); }

	Utils::Vec2I GetWindowSize() const;

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	bool NewFrame();
	void PresentDevice();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);
	::EffekseerRendererGL::RendererRef GetEffekseerRenderer() { return efkRenderer; }
};
