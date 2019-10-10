#pragma once

#include <Effekseer.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

//! TODO : it should be generate renderer.h
#include <EffekseerRendererGL.h>

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

struct EffectPlatformInitializingParameter
{
	bool VSync = true;
};

class EffectPlatform
{
private:
	bool isOpenGLMode_ = false;
	GLFWwindow* window_ = nullptr;
	Effekseer::Manager* manager_ = nullptr;
	EffekseerRenderer::Renderer* renderer_ = nullptr;
	std::vector<Effekseer::Handle> effectHandles_;

	void CreateCheckeredPattern(int width, int height, uint32_t* pixels);

protected:
	std::vector<Effekseer::Effect*> effects_;
	std::vector<std::vector<uint8_t>> buffers_;
	std::vector<uint32_t> checkeredPattern_;

	EffekseerRenderer::Renderer* GetRenderer() const;
	void* GetNativePtr(int32_t index);
	virtual EffekseerRenderer::Renderer* CreateRenderer() = 0;
	virtual void InitializeDevice(const EffectPlatformInitializingParameter& param) {}
	virtual void BeginRendering() {}
	virtual void EndRendering() {}
	virtual void Present() {}

public:
	EffectPlatform(bool isOpenGLMode);
	virtual ~EffectPlatform();

	void Initialize(const EffectPlatformInitializingParameter& param);

	Effekseer::Handle Play(const char16_t* path);

	bool Update();

	virtual bool TakeScreenshot(const char* path) { return false; }

	virtual bool SetFullscreen(bool isFullscreen) { return false; }
};