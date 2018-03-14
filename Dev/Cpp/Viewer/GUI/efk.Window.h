
#pragma once

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1

#if _DEBUG
#pragma comment(lib,"x86/Debug/glfw3.lib")
#else
#pragma comment(lib,"x86/Release/glfw3.lib")
#endif

#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <codecvt>
#include <locale>
#include <functional>

namespace efk
{
	void GLFLW_ResizeCallback(GLFWwindow* w, int x, int y);

	class Window
	{
	private:
		GLFWwindow*	window = nullptr;
		bool		isOpenGLMode = false;

#ifndef _WIN32
        GLuint      vao;
#endif
        
	public:
		Window();
		virtual ~Window();

		bool Initialize(const char16_t* title, int32_t width, int32_t height, bool isSRGBMode, bool isOpenGLMode);

		void Terminate();

		bool DoEvents();

		void SetTitle(const char16_t* title);

		void SetSize(int32_t width, int32_t height);

		void Present();

		void Close();

		void MakeCurrent();

		void MakeNone();

		GLFWwindow*	GetGLFWWindows() const { return window; }

		void* GetNativeHandle() const;

		std::function<void(int, int)> Resized;
	};

}
