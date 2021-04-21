#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace Effekseer
{

class MainWindowState
{
public:
	int32_t PosX = 0;
	int32_t PosY = 0;
	int32_t Width = 0;
	int32_t Height = 0;
	bool IsMaximumMode = false;
	bool IsFrameless = false;
};

class MainWindow
{
private:
	static std::shared_ptr<MainWindow> instance_;
	GLFWwindow* window_ = nullptr;
	bool isOpenGLMode_ = false;
	float dpiScale_ = 1.0f;
	bool isFrameless = false;

	static void GLFW_ContentScaleCallback(GLFWwindow* w, float xscale, float yscale);

	bool InitializeInternal(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode);

public:
	MainWindow();
	virtual ~MainWindow();
	static std::shared_ptr<MainWindow> GetInstance();

	MainWindowState GetState();

	void SetState(const MainWindowState& state);

	float GetDPIScale() const
	{
		return dpiScale_;
	}

	bool IsFrameless() const
	{
		return isFrameless;
	}

	static bool Initialize(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode);

	static void Terminate();

#ifndef SWIG
	std::function<void(float)> DpiChanged;

	GLFWwindow* GetGLFWWindows() const
	{
		return window_;
	}
#endif // !SWIG
};

} // namespace Effekseer