
#pragma once

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#include <GLFW\glfw3.h>
#include <GLFW\glfw3native.h>

#include <codecvt>
#include <locale>

#if _DEBUG
#pragma comment(lib,"x86/Debug/glfw3.lib")
#else
#pragma comment(lib,"x86/Release/glfw3.lib")
#endif

namespace efk
{

	class Window
	{
	private:
		GLFWwindow*	window = nullptr;
		bool		isOpenGLMode = false;

	public:
		Window();
		virtual ~Window();

		bool Initialize(const char16_t* title, int32_t width, int32_t height, bool isSRGBMode, bool isOpenGLMode);

		void Terminate();

		bool DoEvents();

		void SetSize(int32_t width, int32_t height);

		void Present();

		void Close();

		void MakeCurrent();

		void MakeNone();

		GLFWwindow*	GetGLFWWindows() const { return window; }

		void* GetNativeHandle() const;
	};

}