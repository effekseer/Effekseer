#include "efk.Window.h"
#include "../Effekseer/Effekseer/Effekseer.DefaultFile.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <memory>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#endif

namespace efk
{
// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

static std::string utf16_to_utf8(const std::u16string& s)
{
#if defined(_WIN32)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
	auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
	return conv.to_bytes(p, p + s.length());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
	return conv.to_bytes(s);
#endif
}

void GLFW_ResizeCallback(GLFWwindow* w, int x, int y)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);

	if (w_->Resized != nullptr)
	{
		w_->Resized(x, y);
	}
}

void GLFW_DropCallback(GLFWwindow* w, int count, const char** paths)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);
	w_->Droped(paths[0]);
}

void GLFW_CloseCallback(GLFWwindow* w)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);

	if (w_->Closing != nullptr)
	{
		auto ret = w_->Closing();
		glfwSetWindowShouldClose(w, ret);
	}
	else
	{
		glfwSetWindowShouldClose(w, GL_TRUE);
	}
}

void GLFW_WindowFocusCallback(GLFWwindow* gltfWindow, int focused)
{
	auto window = (Window*)glfwGetWindowUserPointer(gltfWindow);

	if (focused)
	{
		if (window->Focused != nullptr)
		{
			window->Focused();
		}

		// If there is modal window, it will be focused.
		ImGuiContext& g = *GImGui;
		for (size_t i = 0; i < g.Viewports.size(); i++)
		{
			const auto viewport = g.Viewports[i];
			if (viewport->Window != nullptr && (viewport->Window->Flags & ImGuiWindowFlags_Modal) != 0)
			{
				glfwFocusWindow((GLFWwindow*)viewport->PlatformHandle);
			}
		}
	}
}

void Window::GLFW_IconifyCallback(GLFWwindow* w, int f)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);

	if (f > 0 && w_->Iconify != nullptr)
	{
		w_->Iconify(f);
	}

	w_->minimized = f != 0;
}

void Window::GLFW_MaximizeCallback(GLFWwindow* w, int f)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);

	w_->maximized = f != 0;
}

void GLFW_ContentScaleCallback(GLFWwindow* w, float xscale, float yscale)
{
	auto w_ = (Window*)glfwGetWindowUserPointer(w);
	if (w_->DpiChanged != nullptr)
	{
		w_->DpiChanged(xscale);
	}
}

Window::Window()
{
}

Window::~Window()
{
}

bool Window::Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, DeviceType deviceType)
{
	this->deviceType = deviceType;

	window = mainWindow->GetGLFWWindows();
	mainWindow_ = mainWindow;

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, GLFW_ResizeCallback);
	glfwSetDropCallback(window, GLFW_DropCallback);
	glfwSetWindowFocusCallback(window, GLFW_WindowFocusCallback);
	glfwSetWindowCloseCallback(window, GLFW_CloseCallback);
	glfwSetWindowIconifyCallback(window, GLFW_IconifyCallback);
	glfwSetWindowMaximizeCallback(window, GLFW_MaximizeCallback);
	// glfwSetWindowContentScaleCallback(window, GLFW_ContentScaleCallback);
	glfwSetWindowSizeLimits(window, 320, 240, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED) > 0;

	mainWindow_->DpiChanged = [this](float scale) -> void { GLFW_ContentScaleCallback(window, scale, 1.0f); };

#ifdef _WIN32
	if (mainWindow->IsFrameless())
	{
		HWND hwnd = glfwGetWin32Window(window);
		vertResize = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

		glfwWndProc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
		::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
		::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

		DWORD wndStyle = ::GetWindowLong(hwnd, GWL_STYLE);
		wndStyle |= WS_SIZEBOX | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		::SetWindowLong(hwnd, GWL_STYLE, wndStyle);
	}
#endif

	return true;
}

void Window::Terminate()
{
#ifdef _WIN32
	if (glfwWndProc)
	{
		HWND hwnd = glfwGetWin32Window(window);
		::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)glfwWndProc);
	}
	if (vertResize)
	{
		glfwDestroyCursor(vertResize);
		vertResize = nullptr;
	}
#endif
	window = nullptr;
	mainWindow_ = nullptr;
}

bool Window::DoEvents()
{
	if (window == nullptr)
		return false;

	if (glfwWindowShouldClose(window))
	{
		return false;
	}

	glfwPollEvents();
	return true;
}

void Window::SetTitle(const char16_t* title)
{
	glfwSetWindowTitle(window, utf16_to_utf8(title).c_str());
}

void Window::SetWindowIcon(const char16_t* iconPath)
{
	Effekseer::DefaultFileInterface file;
	std::unique_ptr<Effekseer::FileReader> reader(file.OpenRead(iconPath));
	if (reader.get() != nullptr)
	{
		size_t size = reader->GetLength();
		std::vector<uint8_t> data(size);
		reader->Read(&data[0], size);

		auto pngLoader = EffekseerRenderer::PngTextureLoader();
		pngLoader.Load(&data[0], size, false);

		GLFWimage image;
		image.pixels = pngLoader.GetData().data();
		image.width = pngLoader.GetWidth();
		image.height = pngLoader.GetHeight();
		glfwSetWindowIcon(window, 1, &image);
	}
}

Vec2 Window::GetSize() const
{
	int32_t w, h;
	glfwGetWindowSize(window, &w, &h);
	Vec2 s;
	s.X = (float)w;
	s.Y = (float)h;
	return s;
}

void Window::SetSize(int32_t width, int32_t height)
{
	glfwSetWindowSize(window, width, height);
}

void Window::Present()
{
	glfwSwapBuffers(window);
}

void Window::Close()
{
	//lfwSetWindowShouldClose(window, 1);
	GLFW_CloseCallback(window);
}

bool Window::IsWindowMaximized() const
{
	return maximized;
}

void Window::SetWindowMaximized(bool maximized)
{
	if (maximized)
	{
		glfwMaximizeWindow(window);
	}
	else
	{
		glfwRestoreWindow(window);
	}
}

bool Window::IsWindowMinimized() const
{
	return minimized;
}

void Window::SetWindowMinimized(bool minimized)
{
	if (minimized)
	{
		glfwIconifyWindow(window);
	}
	else
	{
		glfwRestoreWindow(window);
	}
}

Vec2 Window::GetMousePosition()
{
	double xpos, ypos = 0;
	glfwGetCursorPos(window, &xpos, &ypos);

	Vec2 ret;
	ret.X = (float)xpos;
	ret.Y = (float)ypos;

	return ret;
}

int Window::GetMouseButton(int32_t mouseButton)
{
	return glfwGetMouseButton(window, mouseButton);
}

void Window::MakeCurrent()
{
	if (isOpenGLMode)
	{
		glfwMakeContextCurrent(window);

#ifndef _WIN32
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
#endif
	}
}

void Window::MakeNone()
{
	if (isOpenGLMode)
	{

#ifndef _WIN32
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vao);
#endif
		glfwMakeContextCurrent(nullptr);
	}
}

void* Window::GetNativeHandle() const
{
#ifdef _WIN32
	return glfwGetWin32Window(window);
#else
	return nullptr;
#endif
}

#ifdef _WIN32
LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Window* window = (Window*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case WM_NCCALCSIZE:
	{
		WINDOWPLACEMENT placement;
		GetWindowPlacement(hwnd, &placement);
		if (placement.showCmd == SW_NORMAL)
		{
			NCCALCSIZE_PARAMS* pncsp = (NCCALCSIZE_PARAMS*)lparam;
			pncsp->rgrc[0].top += 0;
			pncsp->rgrc[0].bottom -= 8;
			pncsp->rgrc[0].left += 8;
			pncsp->rgrc[0].right -= 8;
		}
	}
		return 0;
	case WM_NCHITTEST:
	{
		long x = LOWORD(lparam);
		long y = HIWORD(lparam);

		POINT point = {x, y};
		ScreenToClient(hwnd, &point);

		RECT rect;
		GetClientRect(hwnd, &rect);
		if (point.x >= rect.left && point.x < rect.right)
		{

			if (point.y >= rect.top && point.y < rect.top + 6 && !window->maximized)
			{
				glfwSetCursor(window->window, window->vertResize);
				return HTTOP;
			}
			else if (point.y >= rect.top && point.y < rect.top + 32 * window->mainWindow_->GetDPIScale() && ImGui::GetHoveredID() == 0 && GImGui->HoveredWindow == ImGui::FindWindowByName("##MainMenuBar"))
			{
				return HTCAPTION;
			}
		}
	}
	break;
	case WM_NCRBUTTONDOWN:
		if (wparam == HTCAPTION)
		{
			WPARAM cmd = TrackPopupMenu(GetSystemMenu(hwnd, 0), TPM_RETURNCMD, LOWORD(lparam), HIWORD(lparam), 0, hwnd, nullptr);
			if (cmd)
				PostMessage(hwnd, WM_SYSCOMMAND, cmd, 0);
			return 0;
		}
		break;
	}

	return ::CallWindowProc(window->glfwWndProc, hwnd, msg, wparam, lparam);
}
#endif
} // namespace efk
