
#include "efk.Window.h"

namespace efk
{
	// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
	static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

	static std::string utf16_to_utf8(const std::u16string& s)
	{
#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return conv.to_bytes(p, p + s.length());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.to_bytes(s);
#endif
	}

	Window::Window()
	{}

	Window::~Window()
	{}


	bool Window::Initialize(const char16_t* title, int32_t width, int32_t height, bool isSRGBMode, bool isOpenGLMode)
	{
		if (!glfwInit())
		{
			return false;
		}

#ifdef __APPLE__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		this->isOpenGLMode = isOpenGLMode;

		if (this->isOpenGLMode)
		{
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

		window = glfwCreateWindow(width, height, utf16_to_utf8(title).c_str(), nullptr, nullptr);
		if (window == nullptr)
		{
			glfwTerminate();
			return false;
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		return true;
	}


	void Window::Terminate()
	{
		if (window != nullptr)
		{
			glfwDestroyWindow(window);
			window = nullptr;
			glfwTerminate();
		}
	}

	bool Window::DoEvents()
	{
		if (window == nullptr) return false;

		if (glfwWindowShouldClose(window))
		{
			return false;
		}

		glfwPollEvents();
		return true;
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
		glfwSetWindowShouldClose(window, 1);
	}

	void Window::MakeCurrent()
	{
		if (isOpenGLMode)
		{
			glfwMakeContextCurrent(window);
		}
	}

	void Window::MakeNone()
	{
		if (isOpenGLMode)
		{
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
}