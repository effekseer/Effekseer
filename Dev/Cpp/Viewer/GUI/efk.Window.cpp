#include <memory>
#include "efk.Window.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../Effekseer/Effekseer/Effekseer.DefaultFile.h"

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
#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return conv.to_bytes(p, p + s.length());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.to_bytes(s);
#endif
	}

	void GLFLW_ResizeCallback(GLFWwindow* w, int x, int y)
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

	void GLFLW_CloseCallback(GLFWwindow* w)
	{
		auto w_ = (Window*)glfwGetWindowUserPointer(w);
        
        if(w_->Closing != nullptr)
        {
            auto ret = w_->Closing();
            glfwSetWindowShouldClose(w, ret);
        }
		else
        {
            glfwSetWindowShouldClose(w, GL_TRUE);
        }
	}

	void GLFW_WindowFocusCallback(GLFWwindow* w, int f)
	{

		auto w_ = (Window*)glfwGetWindowUserPointer(w);

		if (f > 0 && w_->Focused != nullptr)
		{
			w_->Focused();
		}
	}

	void GLFW_IconifyCallback(GLFWwindow* w, int f)
	{
		auto w_ = (Window*)glfwGetWindowUserPointer(w);

		if (f > 0 && w_->Iconify != nullptr)
		{
			w_->Iconify(f);
		}
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
	{}

	Window::~Window()
	{}


	bool Window::Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, DeviceType deviceType)
	{
		this->deviceType = deviceType;

		window = mainWindow->GetGLFWWindows();
		mainWindow_ = mainWindow;

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, GLFLW_ResizeCallback);
		glfwSetDropCallback(window, GLFW_DropCallback);
		glfwSetWindowFocusCallback(window, GLFW_WindowFocusCallback);
		glfwSetWindowCloseCallback(window, GLFLW_CloseCallback);
		glfwSetWindowIconifyCallback(window, GLFW_IconifyCallback);
		//glfwSetWindowContentScaleCallback(window, GLFW_ContentScaleCallback);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		mainWindow_->DpiChanged = [this](float scale) -> void { 
			GLFW_ContentScaleCallback(window, scale, 1.0f);
		};

		return true;
	}


	void Window::Terminate()
	{ 
		window = nullptr;
		mainWindow_ = nullptr;
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

	void Window::SetTitle(const char16_t* title)
	{
		glfwSetWindowTitle(window, utf16_to_utf8(title).c_str());
	}

	void Window::SetWindowIcon(const char16_t * iconPath)
	{
		Effekseer::DefaultFileInterface file;
		std::unique_ptr<Effekseer::FileReader> 
			reader( file.OpenRead( iconPath ) );
		if( reader.get() != NULL ) {
			size_t size = reader->GetLength();
			std::vector<uint8_t> data(size);
			reader->Read( &data[0], size );

			auto pngLoader = EffekseerRenderer::PngTextureLoader();
			pngLoader.Initialize();
			pngLoader.Load(&data[0], size, false);
			pngLoader.Finalize();

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
		s.X = w;
		s.Y = h;
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
		glfwSetWindowShouldClose(window, 1);
	}

	Vec2 Window::GetMousePosition()
	{
		double xpos, ypos = 0;
		glfwGetCursorPos(window, &xpos, &ypos);

		Vec2 ret;
		ret.X = xpos;
		ret.Y = ypos;

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
}
