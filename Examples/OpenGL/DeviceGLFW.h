#pragma once

#define USE_OPENAL 0

#include <GLFW/glfw3.h>

#include <EffekseerRendererGL.h>
#if USE_OPENAL
#include <EffekseerRendererAL.h>
#endif

#include "../Utils/Input.h"
#include "../Utils/Window.h"

class DeviceGLFW
{
private:
	GLFWwindow* glfwWindow = nullptr;

	::EffekseerRendererGL::OpenGLDeviceType deviceType = ::EffekseerRendererGL::OpenGLDeviceType::OpenGL3;
	::EffekseerRendererGL::RendererRef efkRenderer;

#if USE_OPENAL
	::EffekseerSound::SoundRef efkSound;
#endif

public:
	DeviceGLFW() = default;
	~DeviceGLFW()
	{
		Terminate();
	}

	Utils::Vec2I GetWindowSize() const;

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	bool NewFrame();
	void PresentDevice();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);
	::EffekseerRendererGL::RendererRef GetEffekseerRenderer()
	{
		return efkRenderer;
	}
};
