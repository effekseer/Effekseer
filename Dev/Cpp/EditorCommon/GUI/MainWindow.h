#pragma once

#include <GLFW/glfw3.h>
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
};

class MainWindow
{
private:
	static std::shared_ptr<MainWindow> instance_;
	GLFWwindow* window_ = nullptr;
	bool isOpenGLMode_ = false;

	bool InitializeInternal(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode);

public:
	MainWindow();
	virtual ~MainWindow();
	static std::shared_ptr<MainWindow> GetInstance();

	MainWindowState GetState();
	
	void SetState(const MainWindowState& state);

	static bool Initialize(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode);
	
	static void Terminate();

#ifndef SWIG
	GLFWwindow* GetGLFWWindows() const { return window_; }
#endif // !SWIG
};

} // namespace Effekseer