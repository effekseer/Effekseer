#include "MainWindow.h"
#include "../Common/StringHelper.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Effekseer
{

void MainWindow::GLFW_ContentScaleCallback(GLFWwindow* w, float xscale, float yscale)
{
	auto mainWindow = (MainWindow*)instance_.get();
	// auto w_ = (Window*)glfwGetWindowUserPointer(w);

	mainWindow->dpiScale_ = xscale;
	if (mainWindow->DpiChanged != nullptr)
	{
		mainWindow->DpiChanged(xscale);
	}
}

std::shared_ptr<MainWindow> MainWindow::instance_ = nullptr;

bool MainWindow::InitializeInternal(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode)
{
	if (!glfwInit())
	{
		return false;
	}

	if (isOpenGLMode)
	{
#if defined(__APPLE__) || defined(__linux__)
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	}
	else
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

	if (isSRGBMode)
	{
		glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	}

	isFrameless = state.IsFrameless;
	glfwWindowHint(GLFW_DECORATED, state.IsFrameless ? 0 : 1);
	glfwWindowHint(GLFW_MAXIMIZED, state.IsMaximumMode ? 1 : 0);
	auto window = glfwCreateWindow(state.Width, state.Height, utf16_to_utf8(title).c_str(), nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	window_ = window;

	if (state.PosX > -1000 && state.IsMaximumMode == 0)
	{
		glfwSetWindowPos(window_, state.PosX, state.PosY);
	}

	glfwSetWindowContentScaleCallback(window_, GLFW_ContentScaleCallback);

#ifdef _WIN32
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	float xscale = 1.0f;
	float yscale = 1.0f;
	glfwGetMonitorContentScale(primary, &xscale, &yscale);
	dpiScale_ = xscale;
#endif

	return true;
}

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
	if (window_ != nullptr)
	{
		glfwDestroyWindow(window_);
		window_ = nullptr;
		glfwTerminate();
	}
}

std::shared_ptr<MainWindow> MainWindow::GetInstance()
{
	return instance_;
}

bool MainWindow::Initialize(const char16_t* title, MainWindowState state, bool isSRGBMode, bool isOpenGLMode)
{
	auto instance = std::make_shared<MainWindow>();
	if (instance->InitializeInternal(title, state, isSRGBMode, isOpenGLMode))
	{
		instance_ = instance;
		return true;
	}

	return false;
}

MainWindowState MainWindow::GetState()
{
	MainWindowState state;
	glfwGetWindowSize(window_, &state.Width, &state.Height);
	glfwGetWindowPos(window_, &state.PosX, &state.PosY);
	state.IsMaximumMode = glfwGetWindowAttrib(window_, GLFW_MAXIMIZED) > 0;
	state.IsFrameless = glfwGetWindowAttrib(window_, GLFW_DECORATED) == 0;
	return state;
}

void MainWindow::SetState(const MainWindowState& state)
{
	glfwSetWindowSize(window_, state.Width, state.Height);

	if (!state.IsMaximumMode)
	{
		glfwSetWindowPos(window_, state.PosX, state.PosY);
	}

	glfwSetWindowAttrib(window_, GLFW_MAXIMIZED, state.IsMaximumMode ? 1 : 0);
}

void MainWindow::Terminate()
{
	instance_ = nullptr;
}

} // namespace Effekseer
